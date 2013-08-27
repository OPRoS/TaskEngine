#ifndef __THREADDEF_H__
#define __THREADDEF_H__

static int RES = -1;

#ifdef _WIN32
//#define _WIN32_WINNT 0x500 /* Require Windows NT5 (2K, XP, 2K3) */
#include <windows.h>
#include <process.h>
#include <time.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

/* Thread management macros */
#ifdef _WIN32
/* Win32 */
//#define _WIN32_WINNT 0x500 /* WINBASE.H - Enable SignalObjectAndWait */
typedef unsigned (__stdcall *PTHREAD_FUNC) (void *);
#define THREAD_FUNCTION DWORD WINAPI
#define THREAD_FUNCTION_RETURN DWORD
#define THREAD_SPECIFIC_INDEX DWORD
#define pthread_t HANDLE
#define pthread_attr_t DWORD
#define pthread_create(thhandle,attr,thfunc,tharg) (int)((*thhandle=(HANDLE)_beginthreadex(NULL,0,(PTHREAD_FUNC)thfunc,tharg,0,NULL))==NULL)
#define pthread_join(thread, result) ((WaitForSingleObject((thread),INFINITE)!=WAIT_OBJECT_0) || !CloseHandle(thread))
#define pthread_detach(thread) if(thread!=NULL)CloseHandle(thread)
#define pthread_exit(retval) _endthreadex(0)
#define thread_sleep(nms) Sleep(nms)
#define pthread_cancel(thread) TerminateThread(thread,0)
#define ts_key_create(ts_key, destructor) {ts_key = TlsAlloc();}
#define pthread_getspecific(ts_key) TlsGetValue(ts_key)
#define pthread_setspecific(ts_key, value) TlsSetValue(ts_key, (void *)value)
#define pthread_self() GetCurrentThreadId()
#else
/* pthreads */
/* Nearly everything is already defined */
#define THREAD_FUNCTION void *
#define THREAD_FUNCTION_RETURN void *
#define THREAD_SPECIFIC_INDEX pthread_key_t
//#define thread_sleep(nms) sleep((nms+500)/1000)
#define ts_key_create(ts_key, destructor) pthread_key_create (&(ts_key), destructor);
#define thread_sleep(x)                                                     \
    do{                                                              \
        struct timespec interval;                                    \
        interval.tv_sec = (unsigned int)((x)/1000);                  \
        interval.tv_nsec = ((x)-(interval.tv_sec*1000)) * 1000000;   \
		nanosleep(&interval, NULL);                            		 \
    }while(0)
#endif

/* Syncrhronization macros: Win32 and Pthreads */
#ifdef _WIN32

typedef struct{
	int waiters_count_;
	// Number of waiting threads.
	CRITICAL_SECTION waiters_count_lock_;
	// Serialize access to <waiters_count_>.
	HANDLE sema_;
	// Semaphore used to queue up threads waiting for the condition to
	// become signaled. 
	HANDLE waiters_done_;
	// An auto-reset event used by the broadcast/signal thread to wait
	// for all the waiting thread(s) to wake up and be released from the
	// semaphore. 
	size_t was_broadcast_;
	// Keeps track of whether we were broadcasting or signaling.  This
	// allows us to optimize the code if we're just signaling.
} pthread_cond_t;

typedef struct{
	long tv_sec;	/* seconds */
	long tv_nsec;	/* nanoseconds */
}timespec;

// epoch time으로 변환할 상수
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif

struct timezone{
	int  tz_minuteswest; /* minutes W of Greenwich */
	int  tz_dsttime;     /* type of dst correction */
};

#define pthread_mutex_t HANDLE
//#define pthread_cond_t HANDLE
#define pthread_mutex_lock(pobject) WaitForSingleObject(*pobject,INFINITE)
#define pthread_mutex_unlock(pobject) ReleaseMutex(*pobject)
#define pthread_mutex_init(pobject,pattr) (*pobject=CreateMutex(NULL,FALSE,NULL))
//#define pthread_cond_init(pobject,pattr) (*pobject=CreateEvent(NULL,FALSE,FALSE,NULL))
#define pthread_mutex_destroy(pobject) CloseHandle(*pobject)
#define pthread_cond_destroy(pobject) {DeleteCriticalSection(&((*pobject).waiters_count_lock_));CloseHandle((*pobject).sema_);CloseHandle((*pobject).waiters_done_);}
#define CV_TIMEOUT INFINITE /* Tunable value */
/* USE THE FOLLOWING FOR WINDOWS 9X */
/* For addtional explanation of the condition variable emulation and the use of the
* timeout, see the paper "Batons: A Sequential Synchronization Object" 
* by Andrew Tucker and Johnson M Hart. (Windows Developer뭩 Journal, 
* July, 2001, pp24 ff. www.wdj.com). */
//#define pthread_cond_wait(pcv,pmutex) {ReleaseMutex(*pmutex);WaitForSingleObject(*pcv,CV_TIMEOUT);WaitForSingleObject(*pmutex,INFINITE);};
/* You can use the following on Windows NT/2000/XP and avoid the timeout */
//#define pthread_cond_wait(pcv,pmutex) {SignalObjectAndWait(*pmutex,pcv.events_[SIGNAL],INFINITE,FALSE);WaitForMultipleObjects (2, pcv.events_, FALSE,INFINITE);};
//#define pthread_cond_timedwait(pcv,pmutex,timeout) {RES =SignalObjectAndWait(*pmutex,pcv->sema_,timeout,FALSE);  WaitForSingleObject(*pmutex,timeout);};
//#define pthread_cond_broadcast(pcv) PulseEvent(*(pcv->events_[BROADCAST]))
//#define pthread_cond_signal(pcv) PulseEvent(pcv->events_[SIGNAL])
//static int OnceFlag;
//static DWORD ThId; /* This is ugly, but is required on Win9x for _beginthreadex */

//int pthread_mutex_init	(pthread_mutex_t *, pthread_mutexattr_t *)
//int pthread_mutex_lock	(pthread_mutex_t *);
//int pthread_mutex_unlock	(pthread_mutex_t * mutex) ;
int pthread_cond_init		(pthread_cond_t*, pthread_attr_t*);
int pthread_cond_wait		(pthread_cond_t*, pthread_mutex_t*);
int pthread_cond_timedwait	(pthread_cond_t*, pthread_mutex_t*, timespec*);
int pthread_cond_signal		(pthread_cond_t*);
int pthread_cond_broadcast	(pthread_cond_t*);

int		gettimeofday	(struct timeval *tv, struct timezone *tz);
void	timestamp		(char*);

#else
/* Not Windows. Assume pthreads */

#endif

#endif