/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

//#include "comm/RemoteCLI.h"
#include "log/LogFile.h"
#include "run/control/TaskRunner.h"
#include "run/control/ExeTransition.h"
#include "run/control/ExeBehavior.h"
#include "run/control/ExeTaskTreeNode.h"
#include "run/TaskMem.h"
#include "binding/ModelBinder.h"

#pragma warning (disable : 4290)

class TaskRunner;

class ExeTask : public ExeTaskTreeNode
{
private:
	ModelBinder*   		m_tmPlugin;
	TaskRunner*			m_runner;
	
	std::string			m_curTarget;			//실행중 현재의 target [task.bhv.state.entry]
	std::string			m_startBehavior;	
	RFSMtask*			m_task;
	BehaviorTAB*		m_behaviorTAB;			//task를 실행하는데 사용되는 모든 behavior리스트들
	ConnectorTAB*		m_connTAB;

	//ValueMap*			m_outModelTAB;			//out model값의 임시 저장소, 임시이기 때문에 worker에 저장
	ValueMap*			m_wVarTAB;				//worker 값의 저장소
	MonitorTAB			m_monTargetTAB;	
	bool				m_monitorOff;
	unsigned int		m_monCount;
	long				m_iterCount;

	std::map<std::string, ExeTaskTreeNode*>	m_missionNodes;

	std::set<int>		m_lockedSynchIDs;
	std::set<int>		m_waitingIDs;
	std::map<int, int>	m_waitingCntMap;
	std::map<int, int>::iterator	m_mapiter;

	/* SYNCH */
	std::set<int>					m_SychIdsInTask;  //Task에서 사용하는 모든 synch id들
	std::map<int, pthread_cond_t>	m_SynchIdCondVars;
	std::map<int, pthread_cond_t>	m_SynchIdCondVars2;
	std::map<int, pthread_mutex_t>	m_CondVarsMtxs;
	pthread_cond_t					m_SLEEPstmt_wakeup_cond;
	pthread_mutex_t					m_SLEEPstmt_wakeup_mu;

	void printMsg(std::string msg);

public:
	//RFSMexeWorker(std::string workName, std::string startTask);
	ExeTask(RemoteCLI*, ExeTaskTreeNode*, std::string, int nameLine, std::string, RFSMtask*, ModelBinder*, int, int, unsigned int);
	virtual ~ExeTask(void);

	RFSMtask*			getTask				(void);
	RFSMbehavior*		getBehavior			(std::string);
	RFSMconnector*		getConnector		(std::string);
	RFSMvalueEval		getWorkerVar		(std::string);
	TaskRunner*			getController		(void);
	std::string			getStartBehaviorName(void);
	long				getIterationCount	(void);
	ExeTaskTreeNode*	getMissionNode		(std::string);

	pthread_cond_t*		getSYNCHCond		(int);
	pthread_cond_t*		getSYNCHCond2		(int);
	pthread_mutex_t*	getSYNCHMutex		(int);
	pthread_mutex_t*	getSLEEPStmtMutex	(void);
	pthread_cond_t*		getSLEEPStmtCond	(void);

	

	void setWorkerVar		(std::string varName, RFSMvalueEval);
	void setBehaviorList	(BehaviorTAB*);
	void setConnectorList	(ConnectorTAB*);
	void setTaskSynchIds	(std::set<int>);
	void setMissionNode		(std::string, ExeTaskTreeNode*);

	void releaseCond		(void);
	void breakThreadWait	(void);
	int  waitingCntPlus		(int synchid);
	int	 waitingCntMinus	(int synchid);
	int	 getWaitingCnt		(int synchid);
	void resetWaitingCnt	(void);

	void updateWaitingID	(void);
	bool isWaitingID		(int synchid);
	bool isAllSynchPassed	(int synchid);
	
	void lock_Synch			(int synchid);
	void unlock_Synch		(int synchid);
	bool isLocked_Synch		(int synchid);

	//OUT타입 모델
	//void addOutModelValue(std::string varName, RFSMvalueEval);	//임시로저장
	//RFSMvalueEval getOutModelValue(std::string);
	//int setOutModelValue(); //모델에 모두 반영
	
	void			initMonitorTAB	(void);
	void			setCurTarget	(std::string);
	std::string		getCurTarget	(void);
	RFSMvalueEval	getTaskTime		(int);

	virtual void	cbackBy_Thrfunc	(void);
	int				execute		(void);
	int				runTimer	(void);
	int				forceStop	(void);		

#ifdef _WIN32
	virtual void	cbackBy_Timerfunc(PVOID param, bool timerOrWaitFired);
#endif
};



