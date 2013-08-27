/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

//#pragma warning (disable : 4290)
#include "run/control/ExeBehavior.h"
#include "run/control/DbgTable.h"
#include "run/TaskConfig.h"
#include "run/TaskMem.h"
#include "run/control/ExeTask.h"
#include "run/control/ExeTaskRoot.h"

#include "runtime/symtab/SymTAB4Task.h"
#include "runtime/symtab/TABcall.h"
#include "runtime/symtab/TABsymbol.h"
#include "runtime/rtmodel/RFSMbehavior.h"
#include "runtime/rtmodel/RFSMtreenode.h"
#include "runtime/rtmodel/RFSMconnector.h"
#include "runtime/rtmodel/RFSMtreenode.h"
//#include "runtime/RTObjBuilder.h"
#include "common/GerrorHandler.h"
#include "log/Log.h"
#include "ds/Estring.h"
#include "binding/ModelBinder.h"

//#include "binding/codegen/DirectBinderWriter.h"
#include <typeinfo>
#include <map>
#include <vector>
#include <string>
#include <time.h>

#include "scenario/TPLGenerator.h"

class TaskManager
{
private:
	Log 				m_log;
	ModelBinder* 		m_tmPlugin;
	TaskMem*			m_taskMem;
	std::ostream*		m_ostr;
	RemoteCLI*			m_remoteCLI;
//	DirectBinderWriter*	m_codegen;
	pthread_mutex_t		m_mu;

	std::map<std::string, ExeTaskRoot*>	m_workexeTAB;	//현재 실행중인 worker들

	//int		addExeWorker	(ExeTask* exeWorker); //현재 실행 중인 Task 제어 객체를 저장
	void		setWorkerName	(RFSMtreenode* task, std::string workName, EList<std::string>& tlist); //behavior를 어떤 task에서 사용(참조)하는지를 기록, tlist는 dot파일에 pre declaration을 목적으로
	void		setSubNodes		(RFSMtreenode* node);
	std::string getDot			(std::string workerName, RFSMtreenode* startnode, EList<std::string> tlist);
	std::map<int, int> checkValidSynchIDs(RFSMconnector*);	
	

public:
	TaskManager(ModelBinder* po, RemoteCLI*, std::ostream*);
	virtual ~TaskManager(void);

	std::string getLoadedWorkersInfo	(void);
	std::string getRunningWorkersInfo	(void);
	std::string getBehaviorsInfo		(void);
	std::string getConnectorsInfo		(void);
	std::string getRunningWorkersList	(void);
	std::string getLoadedWorkersList	(void);

	int unloadBehavior	(std::string behaviorName);	
	int unloadConnector	(std::string cName);
	int loadWorker		(std::string workerFileName, OSFileSys*);
	int unloadWorker	(std::string workName);
	int startTask		(std::string workName, int exeMode);
	//int updateWorker	(std::string workName);
	//int updateWorkers	(void);
	int stopWorker		(std::string workName);
	int	stopAllWorker	(void);
	bool isTaskRunning	(void); //현재 실행 중인 태스크가 존재하는지 검사
	ExeTask*	getExeTask	(std::string tName);
	RFSMtask*	getTask		(std::string tName);
	


	// 리턴되는 정수값들이 0이면 정상적인 수행이고
	// 1이상의 정수이면 에러코드이다.
};
