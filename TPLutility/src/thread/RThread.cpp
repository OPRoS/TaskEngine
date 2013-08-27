/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "thread/RThread.h"

//pthread_attr_t  RThread::s_attr;
int RThread::s_scope = 0;

RThread::RThread(std::string clsname, unsigned int timePeriod)
{
	//m_cond = PTHREAD_COND_INITIALIZER;
	//m_lmu = PTHREAD_MUTEX_INITIALIZER;
	//m_lmu = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	//pthread_mutex_init(m_lmu, NULL);
	
	m_className	= clsname;
	m_timePeriod = timePeriod;
	m_runnable	= NULL;
	m_threadId	= NULL;
	m_thType	= RThread::ATTACH;

	pthread_cond_init (&m_cond, NULL);
	pthread_mutex_init(&m_lmu, NULL);


#ifdef _WIN32 	
	m_TRrunning = false;
	pthread_mutex_init(&m_tmu, NULL);
#endif
}

RThread:: ~RThread(void)
{
	//cleanUp();
	//free(m_lmu);
	pthread_mutex_unlock(&m_lmu);
	pthread_mutex_destroy(&m_lmu);
	pthread_cond_signal(&m_cond);
	pthread_cond_destroy(&m_cond);
}

void RThread::setThrdTraceOn()
{
	m_threadTraceOn = true;
}

void RThread::setThrdTraceOff()
{
	m_threadTraceOn = false;
}

std::string RThread::getClassName()
{
	return m_className;
}

std::string RThread::getInstanceName()
{
	return m_instanceName;
}

int RThread::getCreateType()
{
	return m_thType;
}

Log RThread::getLog4Trace()
{
	return m_log4thrd;
}

void* RThread::runFunc(void* self)
{
	RThread* trd = (RThread*)self;
	//pthread_t pid = trd->getThread_t();
	
    //pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

    // 쓰레드 종료시 호출될 함수 등록
	//pthread_cleanup_push(cleanUpFunc, (void *)(&pid));
#ifdef _WIN32
	timeBeginPeriod(1);
#endif
	//쓰레드 실행 함수, run은 virtual이기 때문에 상속받은 클래스에서 정의한 것을 호출
	RThreable* runnable = trd->getRunnable();
	if(runnable==0)
		trd->cbackBy_Thrfunc();
	else
		trd->getRunnable()->run();

	//pthread_cleanup_pop(0);
	pthread_exit(0);
#ifdef _WIN32
	timeEndPeriod(1);
#endif
	return NULL;
}

/*
void RThread::cleanUpFunc(void* self)
{
	int rc;
	RThread* trd = (RThread*)self;
	if(trd->getCreateType() == RThread::ATTACH)
		join(trd);
}
*/
void RThread::join(RThread* trd)
{
	if(trd->getCreateType() != RThread::ATTACH) return;

	pthread_t pid = trd->getThread_t();
	int rc = 0;
	//if(m_runMode = THREAD_){
		int status;
		rc= pthread_join(pid, (void**)&status);
	//}	
	//else if(m_runMode	= QTIMER_){
	//	SetEvent(m_DoneEvent);
	//}
	if (rc == 0){
		//쓰레드가 제대로 동작하는지에 대한 로그를 출력(화면지전분해서 일단)	
		//if(TaskConfig::getIsThreadTrace())
		if(m_threadTraceOn)
			LOG_TRACE(trd->getLog4Trace(), " (THREAD:%s<%s>, %d) is joined", trd->getClassName().c_str(), trd->getInstanceName().c_str(), trd->getThreadNum());
	}
	else{
		LOG_ERROR(trd->getLog4Trace(), " (THREAD:%s<%s>, %d) return code from pthread_join() is %d", trd->getClassName().c_str(), trd->getInstanceName().c_str(), trd->getThreadNum(), rc);
	}
}

int RThread::create_thr(int threadType, std::string instName)
{
/*
	if(isDetached){
		pthread_attr_t tattr;
		int rc = pthread_attr_init(&tattr); //win32는 이러함 함수가 없다...
		//if (rc != 0) {
		//	PrintError("pthread_attr_init failed at", rc, __FILE__, __LINE__);
		//	exit(rc);
		//}
		rc = pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_DETACHED);
		m_threadHandle = pthread_create(&m_threadId, &tattr, runFunc, (void*)this);
	}
	else{
		m_threadHandle = pthread_create(&m_threadId, NULL, runFunc, (void*)this);
		if(m_threadHandle != 0)
			return -1;
		else {		
			//LOG_TRACE(m_log, " (THREAD:%s, %d) is created", m_name.c_str(), m_threadId);
			return 0;
		}
	}
*/
	pthread_attr_t thread_attr;
#ifndef _WIN32
	struct sched_param 	thread_param;
	thread_param.sched_priority = 30; /* setting the Sched policy */
	pthread_attr_init(&thread_attr);
	pthread_attr_setschedpolicy(&thread_attr, SCHED_FIFO);
	pthread_attr_setschedparam(&thread_attr, &thread_param);
	//if(pthread_attr_setinheritsched(&thread_attr, PTHREAD_EXPLICIT_SCHED) != 0){
		/* deals with the error */
	//}
#endif

    m_instanceName = instName;
	m_thType = RThread::ATTACH;
	m_threadHandle = pthread_create(&m_threadId, &thread_attr, runFunc, (void*)this);

#ifdef _WIN32	
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
#endif
	if(m_threadHandle != 0)
		return -1;
	else {
		//if(TaskConfig::getIsThreadTrace())
		if(m_threadTraceOn)
			LOG_TRACE(m_log4thrd, " (THREAD:%s<%s>, %u) is created", m_className.c_str(), instName.c_str(), m_threadId);
		if(threadType==RThread::DETACH){
			m_thType = RThread::DETACH;
			pthread_detach(m_threadId);			
		}
		return 0;
	}
}

pthread_t RThread::getThread_t()
{
	return m_threadId;
}

int RThread::getThreadNum()
{
	if(m_threadId == NULL)
		return 0;
	else{
		int th_num = (int)m_threadId;
		
		if(th_num == 0)//테스팅 목적임..
			printf("RThread::getThreadNum():Thread ID ERROR....\n");

		if(th_num < 0)
			return (-1)*th_num;
		else
			return th_num;
	}
}

void RThread::cbackBy_Thrfunc()
{
	//something to do on a child class
}

int RThread::cancel()
{
	int rc = pthread_cancel(m_threadId);
	if(rc==0){
		int status;
		rc = pthread_join(m_threadId, (void**)&status);
	}
	
	if (rc==0) return 0;
	else return -1;
}

int RThread::wait()
{
	pthread_mutex_lock(&m_lmu);
	pthread_cond_wait(&m_cond, &m_lmu);
	pthread_mutex_unlock(&m_lmu);
	return 0;
}

int RThread::resume()
{
	pthread_mutex_lock(&m_lmu);
	pthread_cond_signal(&m_cond);
	pthread_mutex_unlock(&m_lmu);
	return 0;
}

/**
디폴트는 cleanUp()함수를 호출하는 것이다.
필요하면 하위 클래스에서 재정의
*/

void RThread::setIsRunning(bool val)
{
	m_running = val;
}

int RThread::wait4join()
{
	//something to do;
	join(this);		
	return 0;
}

void RThread::PrintError(char* msg, int status, char* fileName, int lineNumber) {
	std::cout << msg << ' ' << fileName << ":" << lineNumber << " - " << strerror(status) << std::endl;
}

void RThread::setRunnable(RThreable* runnable)
{
	m_runnable = runnable;
}

RThreable* RThread::getRunnable()
{
	return m_runnable;
}

pthread_cond_t*	RThread::getCond()
{
	return &m_cond;
}

pthread_mutex_t* RThread::getMutex()
{
	return &m_lmu;
}

bool RThread::isRunning()
{
	return m_running;
}


#ifdef _WIN32
void* RThread::timerRoutine	(void* self)
{
	RThread* trd = (RThread*)self;
	
	if(trd->m_TRrunning){
		/* //plot을 그릴 때 필요 없어서 마킹, 스킵 빈도를 보려면 필요함
		pthread_mutex_lock(&trd->m_tmu);
		PeRecord rec;
		rec.name =  trd->getInstanceName();
		rec.cycle = 0;
		rec.proc = 0;
		PerformTable::addPeRecord(rec);
		pthread_mutex_unlock(&trd->m_tmu);
		*/
		return NULL;
	}

	pthread_mutex_lock(&trd->m_tmu);
	trd->m_TRrunning = true;
	trd->cbackBy_Timerfunc(NULL, false);
	trd->m_TRrunning = false;
	pthread_mutex_unlock(&trd->m_tmu);
	return NULL;
}

int	RThread::start_que(HANDLE timerQueue, std::string name)
{
	m_DoneEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	//큐를 생성
	m_TimerQueue = CreateTimerQueue();
	//m_TimerQueue = NULL;
		
	timeBeginPeriod(1);
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	//큐에 타이머를 add
	if (!CreateTimerQueueTimer(&m_Timer, m_TimerQueue, (WAITORTIMERCALLBACK)timerRoutine, (void*)this, 0, m_timePeriod, WT_EXECUTELONGFUNCTION)){
		printf("CreateTimerQueueTimer failed (%d)\n", GetLastError());
		return -1;
	}
	if (WaitForSingleObject(m_DoneEvent, INFINITE) != WAIT_OBJECT_0)
		printf("WaitForSingleObject failed (%d)\n", GetLastError());
	timeEndPeriod(1);
	
	CloseHandle(m_DoneEvent);

	// Delete all timers in the timer queue.
	if (!DeleteTimerQueue(m_TimerQueue))
		printf("DeleteTimerQueue failed (%d)\n", GetLastError());
	//if (!DeleteTimerQueueTimer(NULL, m_Timer, NULL))
	//	printf("DeleteTimerQueueTimer failed (%d)\n", GetLastError());
	return 0;
}

void RThread::cbackBy_Timerfunc(PVOID param, bool timerOrWaitFired)
{
	//something to do in a child class
}

HANDLE RThread::getTimer(void)
{
	return m_Timer;
}

HANDLE RThread::getTimerQueue(void)
{
	return m_TimerQueue;
}
#endif
