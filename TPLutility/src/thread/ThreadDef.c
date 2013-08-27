/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "thread/ThreadDef.h"

#ifdef _WIN32
int pthread_cond_init (pthread_cond_t *cv, pthread_attr_t * attr)
{
	cv->waiters_count_ = 0;
	cv->was_broadcast_ = 0;
	cv->sema_ = CreateSemaphore (NULL, 0, 0x7fffffff, NULL);      
	InitializeCriticalSection (&cv->waiters_count_lock_);
	cv->waiters_done_ = CreateEvent (NULL, FALSE, FALSE, NULL); 
}

int pthread_cond_wait (pthread_cond_t *cv, pthread_mutex_t *external_mutex)
{
	int last_waiter;

	// Avoid race conditions.
	EnterCriticalSection (&cv->waiters_count_lock_);
	cv->waiters_count_++;
	LeaveCriticalSection (&cv->waiters_count_lock_);

	// This call atomically releases the mutex and waits on the
	// semaphore until <pthread_cond_signal> or <pthread_cond_broadcast>
	// are called by another thread.
	SignalObjectAndWait (*external_mutex, cv->sema_, INFINITE, FALSE);

	// Reacquire lock to avoid race conditions.
	EnterCriticalSection (&cv->waiters_count_lock_);

	// We're no longer waiting...
	cv->waiters_count_--;

	// Check to see if we're the last waiter after <pthread_cond_broadcast>.
	last_waiter = (cv->was_broadcast_ && cv->waiters_count_ == 0);

	LeaveCriticalSection (&cv->waiters_count_lock_);

	// If we're the last waiter thread during this particular broadcast
	// then let all the other threads proceed.
	if (last_waiter)
		// This call atomically signals the <waiters_done_> event and waits until
		// it can acquire the <external_mutex>.  This is required to ensure fairness. 
		SignalObjectAndWait (cv->waiters_done_, *external_mutex, INFINITE, FALSE);
	else
		// Always regain the external mutex since that's the guarantee we
		// give to our callers. 
		WaitForSingleObject (*external_mutex, INFINITE);
}


int pthread_cond_timedwait (pthread_cond_t* cv, pthread_mutex_t* external_mutex, timespec* abstime)
{
	int last_waiter;

	DWORD timeout = 0;

	//struct timeval now;
	//timespec timenow;
	//gettimeofday(&now, NULL);
	//timenow.tv_sec = now.tv_sec; 
	//timenow.tv_nsec = now.tv_usec * 1000;
    //timeout = (DWORD)((abstime->tv_sec - timenow.tv_sec) * 1000 + (abstime->tv_nsec - timenow.tv_nsec) / 1000000 + 5);

	timeout = (DWORD)((abstime->tv_sec) * 1000 + (abstime->tv_nsec) / 1000000 + 5);  

	EnterCriticalSection (&cv->waiters_count_lock_);
	cv->waiters_count_++;
	LeaveCriticalSection (&cv->waiters_count_lock_);

	SignalObjectAndWait (*external_mutex, cv->sema_, timeout, FALSE);
	EnterCriticalSection (&cv->waiters_count_lock_);
	cv->waiters_count_--;
	last_waiter = (cv->was_broadcast_ && cv->waiters_count_ == 0);
	LeaveCriticalSection (&cv->waiters_count_lock_);

	if (last_waiter)
		SignalObjectAndWait (cv->waiters_done_, *external_mutex, INFINITE, FALSE);
	else
		WaitForSingleObject (*external_mutex, INFINITE);
}

int pthread_cond_signal (pthread_cond_t *cv)
{
	int have_waiters;
	EnterCriticalSection (&cv->waiters_count_lock_);
	have_waiters = cv->waiters_count_ > 0;
	LeaveCriticalSection (&cv->waiters_count_lock_);

	// If there aren't any waiters, then this is a no-op.  
	if (have_waiters) ReleaseSemaphore (cv->sema_, 1, 0);
}

int pthread_cond_broadcast (pthread_cond_t *cv)
{
	// This is needed to ensure that <waiters_count_> and <was_broadcast_> are
	// consistent relative to each other.
	int have_waiters = 0;
	EnterCriticalSection (&cv->waiters_count_lock_);

	if (cv->waiters_count_ > 0) {
		// We are broadcasting, even if there is just one waiter...
		// Record that we are broadcasting, which helps optimize
		// <pthread_cond_wait> for the non-broadcast case.
		cv->was_broadcast_ = 1;
		have_waiters = 1;
	}

	if (have_waiters) {
		// Wake up all the waiters atomically.
		ReleaseSemaphore (cv->sema_, cv->waiters_count_, 0);

		LeaveCriticalSection (&cv->waiters_count_lock_);

		// Wait for all the awakened threads to acquire the counting
		// semaphore. 
		WaitForSingleObject (cv->waiters_done_, INFINITE);
		// This assignment is okay, even without the <waiters_count_lock_> held 
		// because no other waiter threads can wake up to access it.
		cv->was_broadcast_ = 0;
	}
	else
		LeaveCriticalSection (&cv->waiters_count_lock_);
}


int gettimeofday(struct timeval *tv, struct timezone *tz)
{
  FILETIME ft;
  unsigned __int64 tmpres = 0;
  static int tzflag;

  if (NULL != tv)
  {
    // system time을 구하기
    GetSystemTimeAsFileTime(&ft);

    // unsigned 64 bit로 만들기
    tmpres |= ft.dwHighDateTime;
    tmpres <<= 32;
    tmpres |= ft.dwLowDateTime;
    // 100nano를 1micro로 변환하기    tmpres /= 10;
    // epoch time으로 변환하기
    tmpres -= DELTA_EPOCH_IN_MICROSECS;    

    // sec와 micorsec으로 맞추기
    tv->tv_sec = (tmpres / 1000000UL);
    tv->tv_usec = (tmpres % 1000000UL);
  }

  // timezone 처리
  if (NULL != tz)
  {
    if (!tzflag){
      _tzset();
      tzflag++;
    }
    tz->tz_minuteswest = _timezone / 60;
    tz->tz_dsttime = _daylight;
  }

  return 0;
}

void timestamp(char* prefix)
{
	struct timeval mytime;
	// 현재 시간을 얻어온다.
	gettimeofday(&mytime, NULL);
	printf("[%s] %ld:%ld\n", prefix, mytime.tv_sec, mytime.tv_usec);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
typedef struct {
    int waiters_count;
    CRITICAL_SECTION waiters_count_lock;
    HANDLE sema;
    HANDLE waiters_done;
    size_t was_broadcast;
} PThreadCond;

int pthread_mutex_init(pthread_mutex_t * mutex, pthread_mutexattr_t * attr) {
    assert(attr == NULL);
    *mutex = (pthread_mutex_t)CreateMutex(NULL, FALSE, NULL);
    if (*mutex == NULL) return set_win32_errno(GetLastError());
    return 0;
}

int pthread_mutex_lock(pthread_mutex_t * mutex) {
    assert(mutex != NULL);
    assert(*mutex != NULL);
    if (WaitForSingleObject(*mutex, INFINITE) == WAIT_FAILED) return set_win32_errno(GetLastError());
    return 0;
}

int pthread_mutex_unlock(pthread_mutex_t * mutex) {
    assert(mutex != NULL);
    assert(*mutex != NULL);
    if (!ReleaseMutex(*mutex)) return set_win32_errno(GetLastError());
    return 0;
}


int pthread_cond_init(pthread_cond_t * cond, pthread_attr_t * attr) {
    PThreadCond * p = (PThreadCond *)loc_alloc_zero(sizeof(PThreadCond));
    assert(attr == NULL);
    p->waiters_count = 0;
    p->was_broadcast = 0;
    p->sema = CreateSemaphore(NULL, 0, 0x7fffffff, NULL);
    if (p->sema == NULL) return set_win32_errno(GetLastError());
    InitializeCriticalSection(&p->waiters_count_lock);
    p->waiters_done = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (p->waiters_done == NULL) return set_win32_errno(GetLastError());
    *cond = (pthread_cond_t)p;
    return 0;
}

int pthread_cond_wait(pthread_cond_t * cond, pthread_mutex_t * mutex) {
    DWORD res = 0;
    int last_waiter = 0;
    PThreadCond * p = (PThreadCond *)*cond;

    EnterCriticalSection(&p->waiters_count_lock);
    p->waiters_count++;
    LeaveCriticalSection(&p->waiters_count_lock);

   
    res = SignalObjectAndWait(*mutex, p->sema, INFINITE, FALSE);
    if (res == WAIT_FAILED) return set_win32_errno(GetLastError());

  
    EnterCriticalSection(&p->waiters_count_lock);

   
    p->waiters_count--;


    last_waiter = p->was_broadcast && p->waiters_count == 0;

    LeaveCriticalSection(&p->waiters_count_lock);

 
    if (last_waiter) {
    
        DWORD err = SignalObjectAndWait(p->waiters_done, *mutex, INFINITE, FALSE);
        if (err == WAIT_FAILED) return set_win32_errno(GetLastError());
    }
    else {
       
        DWORD err = WaitForSingleObject(*mutex, INFINITE);
        if (err == WAIT_FAILED) return set_win32_errno(GetLastError());
    }
    assert(res == WAIT_OBJECT_0);
    return 0;
}

int pthread_cond_signal(pthread_cond_t * cond) {
    int have_waiters = 0;
    PThreadCond * p = (PThreadCond *)*cond;

    EnterCriticalSection(&p->waiters_count_lock);
    have_waiters = p->waiters_count > 0;
    LeaveCriticalSection(&p->waiters_count_lock);

  
    if (have_waiters) {
        if (!ReleaseSemaphore(p->sema, 1, 0)) return set_win32_errno(GetLastError());
    }
    return 0;
}

int pthread_cond_broadcast(pthread_cond_t * cond) {
    int have_waiters = 0;
    PThreadCond * p = (PThreadCond *)*cond;


    EnterCriticalSection(&p->waiters_count_lock);

    if (p->waiters_count > 0) {
      
        p->was_broadcast = 1;
        have_waiters = 1;
    }

    if (have_waiters) {
       
        if (!ReleaseSemaphore(p->sema, p->waiters_count, 0)) return set_win32_errno(GetLastError());

        LeaveCriticalSection(&p->waiters_count_lock);

      
        if (WaitForSingleObject(p->waiters_done, INFINITE) == WAIT_FAILED) return set_win32_errno(GetLastError());
      
        p->was_broadcast = 0;
    }
    else {
        LeaveCriticalSection(&p->waiters_count_lock);
    }
    return 0;
}

/*

int pthread_cond_timedwait(pthread_cond_t * cond, pthread_mutex_t * mutex, const struct timespec * abstime) 
{
    DWORD res = 0;
    int last_waiter = 0;
    PThreadCond * p = (PThreadCond *)*cond;
    DWORD timeout = 0;
    struct timespec timenow;

    if (clock_gettime(CLOCK_REALTIME, &timenow)) return errno;
    if (abstime->tv_sec < timenow.tv_sec) return ETIMEDOUT;
    if (abstime->tv_sec == timenow.tv_sec) {
        if (abstime->tv_nsec <= timenow.tv_nsec) return ETIMEDOUT;
    }
    timeout = (DWORD)((abstime->tv_sec - timenow.tv_sec) * 1000 + (abstime->tv_nsec - timenow.tv_nsec) / 1000000 + 5);

    EnterCriticalSection(&p->waiters_count_lock);
    p->waiters_count++;
    LeaveCriticalSection(&p->waiters_count_lock);

   
    res = SignalObjectAndWait(*mutex, p->sema, timeout, FALSE);
    if (res == WAIT_FAILED) return set_win32_errno(GetLastError());

   
    EnterCriticalSection(&p->waiters_count_lock);

  
    p->waiters_count--;

  
    last_waiter = p->was_broadcast && p->waiters_count == 0;

    LeaveCriticalSection(&p->waiters_count_lock);

  
    if (last_waiter) {
      
        DWORD err = SignalObjectAndWait(p->waiters_done, *mutex, INFINITE, FALSE);
        if (err == WAIT_FAILED) return set_win32_errno(GetLastError());
    }
    else {
      
        DWORD err = WaitForSingleObject(*mutex, INFINITE);
        if (err == WAIT_FAILED) return set_win32_errno(GetLastError());
    }

    if (res == WAIT_TIMEOUT) return errno = ETIMEDOUT;
    assert(res == WAIT_OBJECT_0);
    return 0;
}
*/
#else
int msleep(unsigned long milisec)
{
	 struct timespec ts;
	   ts.tv_sec = milisec / 1000;
	   ts.tv_nsec = (milisec - ts.tv_sec*1000) * 1000000;
	   nanosleep(&ts, NULL);
}
#endif
