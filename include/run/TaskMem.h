/*
 * TaskMem.h
 *
 *  Created on: 2009. 3. 12
 *      Author: jinseo
 */

#ifndef TASKMEM_H_
#define TASKMEM_H_

#include <map>
#include <string>
#include "log/Log.h"
#include "common/GerrorHandler.h"
#include "runtime/symtab/SymTAB4Task.h"
#include "runtime/rtmodel/RFSMbehavior.h"
#include "runtime/rtmodel/RFSMtask.h"
#include "runtime/rtmodel/RFSMconnector.h"
#include "run/TaskConfig.h"
#include "comm/MonitorRequest.h"

class MonitorRec{
public:
	long	iter;	//����͸� ���� Ƚ��
    long	siter;	//���� ���� ���޵� ����͸� Ƚ��
	long	after;	//����͸� ���� �ð�	
	bool	on;

	MonitorRec();
	virtual ~MonitorRec();
};

typedef std::map<std::string, RFSMbehavior*>	BehaviorTAB;
typedef std::map<std::string, RFSMtask*>		TaskTAB;
typedef std::map<std::string, RFSMconnector*>	ConnectorTAB;
typedef std::map<std::string, SymTAB4Task*>		SymTAB;
typedef std::map<std::string, MonitorRec>		MonitorTAB;
typedef std::pair<std::string, MonitorRec>		MonRecPair;

class TaskMem
{
private:
	Log				m_log;
	//StrMap*		m_taskTAB;		//<Task�̸�, ���� behavior�̸�>
	SymTAB*			m_symTAB;		//<Task�̸�, �ɺ����̺�>
	TaskTAB*		m_taskTAB2;
	BehaviorTAB*	m_behaviorTAB;	//<behavior �̸�, behavior ��������>
	ConnectorTAB*	m_connectorTAB;
	MonitorTAB*		m_MonInfoTAB;   //key�� bhv.state.block(task�̸��� ����) 
	                                                  
	//�̰� ����͸� �������� �����ϴµ�.. task�� on off ���̺��� ���� ����(�̶��� task�̸� ���)
	
	
public:
	TaskMem(std::ostream*);
	virtual ~TaskMem();

	int addMonitorInfo(RFSMbehavior* bhv);
	int addSymbolT(std::string, SymTAB4Task*);
	//int addTask(std::string taskName, std::string starbehaviorName);
	int addTask2(std::string, RFSMtask*);
	int addBehavior(RFSMbehavior*);
	int addConnector(RFSMconnector*);

	RFSMbehavior*	findBehavior(std::string behaviorName);
	RFSMconnector*	findConnector(std::string);
	//RFSMbehavior*	getStartBehavior(std::string workName);
	RFSMtask*		getTask2(std::string workName);
	TABsymbol*		getSymbol(std::string workerName, std::string name);

	//StrMap* 		getTaskTAB();
	TaskTAB* 		getTaskTAB2();
	SymTAB*			getSymbolTAB();
	BehaviorTAB*	getBehaviorTAB();
	ConnectorTAB*	getConnectorTAB();
	MonitorTAB*		getMonitorInfoTAB();

	void leakTest();

};

#endif /* TASKMEM_H_ */
