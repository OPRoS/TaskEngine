/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "run/control/ExeTaskRoot.h"

ExeTaskRoot::ExeTaskRoot(ExeTask* eTask, unsigned int tp): RThread("ExeTaskRoot", tp)
{
	m_eTask			= eTask;
	m_runCompleted	= false;
}

ExeTaskRoot::~ExeTaskRoot(void)
{
	if(!m_runCompleted){
		pthread_mutex_lock(&m_lmu);
		pthread_cond_wait(&m_cond, &m_lmu);
		pthread_mutex_unlock(&m_lmu);
	}
	//외부에서 stop하면 m_eTask가 먼저 NULL이 될 수다. 위는 대기문
	if(m_eTask != NULL){
		delete m_eTask;
		m_eTask = NULL;
	}
}

void ExeTaskRoot::cbackBy_Thrfunc(void)
{
	std::string tname = m_eTask->getNodeName();
	m_eTask->execute();
	m_eTask->wait4join();
	if(!m_eTask->isStopped()){
		m_runCompleted = true; //정상 완료일 때
	}
	else{ //외부 stop
		pthread_mutex_lock(&m_lmu);
		pthread_cond_signal(&m_cond);
		pthread_mutex_unlock(&m_lmu);
		m_runCompleted = true;
	}
}

int ExeTaskRoot::execute()
{	
	std::string name = "ExeTaskRoot:" + m_eTask->getNodeName();
	this->create_thr(RThread::ATTACH, name);
	return 0;
}

ExeTask* ExeTaskRoot::getExeTask(void)
{
	return m_eTask;
}