/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

extern "C"{
#include "thread/ThreadDef.h"
}
#include <errno.h>
#include <iostream>
#include "thread/RThreable.h"
//#include "runtime/TaskConfig.h"
//#include "runtime/control/PerformTable.h"
#include "log/Log.h"
#include <string>

class RThread
{
private:	
	Log				m_log4thrd; //Thread 생성 삭제에 대한 trace용
	bool			m_threadTraceOn;
	int				m_threadHandle;	
	int				m_runMode;
	std::string		m_className;
	std::string		m_instanceName;
	RThreable*		m_runnable;	
	pthread_t		m_threadId;
	
	static void* runFunc		(void*);
	//static void  cleanUpFunc	(void*);
	void  join			(RThread*);
	void  cancel		(RThread*);

#ifdef _WIN32 //QueueTimer,WIN
	static void* timerRoutine	(void*);
#endif

protected:
	int					m_thType;
	bool				m_running;
	pthread_cond_t		m_cond;
	pthread_mutex_t		m_lmu;
	unsigned int		m_timePeriod;

	//static pthread_attr_t	s_attr;
	static int			s_scope;

#ifdef _WIN32 //QueueTimer,WIN
	HANDLE	m_DoneEvent;
	HANDLE	m_Timer;
	HANDLE	m_TimerQueue;
	LARGE_INTEGER liCounter0, liCounter1, liCounter2, liFrequency;
	double litemp;
#endif
	
public:
	RThread(std::string, unsigned int timePeriod);
	virtual ~RThread(void);

	static const unsigned int DETACH	= 1;
	static const unsigned int ATTACH    = 2;

	//cleanUpFunc, runFunc함수들이 static이라서 void*로 RThread를 넘기는데 이때  멤버를 접근하기위해 필요
	void		setThrdTraceOn	(void);
	void		setThrdTraceOff	(void);
	std::string	getClassName	(void);
	std::string getInstanceName	(void);
	Log			getLog4Trace	(void);
	int			getCreateType	(void);
	void		PrintError		(char* msg, int status, char* fileName, int lineNumber);

	virtual pthread_t	getThread_t		(void);
	virtual int			getThreadNum	(void);

	pthread_cond_t*		getCond			(void);
	pthread_mutex_t*	getMutex		(void);

	RThreable*			getRunnable		(void);
	void				setRunnable		(RThreable*);
	bool				isRunning		(void);
	void				setIsRunning	(bool);

	virtual int		create_thr		(int threadType, std::string);
	virtual void	cbackBy_Thrfunc	(void);
	virtual int		cancel			(void);
	virtual int		wait4join		(void);
	virtual int		wait			(void);
	virtual int		resume			(void);

#ifdef _WIN32 //QueueTimer,WIN
	virtual int		start_que			(HANDLE, std::string);
	virtual void	cbackBy_Timerfunc	(PVOID param, bool timerOrWaitFired);
	HANDLE getTimer();
	HANDLE getTimerQueue();
	bool  m_TRrunning; //timerRoutine의 실행 중인지 여부
	pthread_mutex_t	m_tmu;
#endif
	
};

//static int THREAD_ = 1;
//static int QTIMER_ = 2;
//static int NOTRUN_ = 3;

/*
1.쓰레드 종료
-pthread_exit 실행
-쓰레드 함수가 리턴되는 경우

2.pthread_create
pthread_join을 해야 쓰레드 자원을 반환(종료시 반환하지 않음, 반드시 종료후에 join)

pthread_create가 PTHREAD_CREATE_DETACHED 특성으로 실행
join을 할 수 없다.(왜냐하면 메인 쓰레드에서 분리 했기 때문, 그러므로 쓰레드 종료되면 모든 자원 반환)


pthread_attr_t tattr;
pthread_t tid;
void *start_routine;
void arg
int ret;

// initialized with default attributes 
ret = pthread_attr_init(&tattr);
ret = pthread_attr_setdetachstate(&tattr,PTHREAD_CREATE_DETACHED);
ret = pthread_create(&tid, &tattr, start_routine, arg);

*/