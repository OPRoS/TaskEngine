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

	std::map<std::string, ExeTaskRoot*>	m_workexeTAB;	//���� �������� worker��

	//int		addExeWorker	(ExeTask* exeWorker); //���� ���� ���� Task ���� ��ü�� ����
	void		setWorkerName	(RFSMtreenode* task, std::string workName, EList<std::string>& tlist); //behavior�� � task���� ���(����)�ϴ����� ���, tlist�� dot���Ͽ� pre declaration�� ��������
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
	bool isTaskRunning	(void); //���� ���� ���� �½�ũ�� �����ϴ��� �˻�
	ExeTask*	getExeTask	(std::string tName);
	RFSMtask*	getTask		(std::string tName);
	


	// ���ϵǴ� ���������� 0�̸� �������� �����̰�
	// 1�̻��� �����̸� �����ڵ��̴�.
};
