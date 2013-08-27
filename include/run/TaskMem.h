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
	long	iter;	//모니터링 전달 횟수
    long	siter;	//현재 까지 전달된 모니터링 횟수
	long	after;	//모니터링 시작 시간	
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
	//StrMap*		m_taskTAB;		//<Task이름, 시작 behavior이름>
	SymTAB*			m_symTAB;		//<Task이름, 심볼테이블>
	TaskTAB*		m_taskTAB2;
	BehaviorTAB*	m_behaviorTAB;	//<behavior 이름, behavior 제어정보>
	ConnectorTAB*	m_connectorTAB;
	MonitorTAB*		m_MonInfoTAB;   //key는 bhv.state.block(task이름이 없다) 
	                                                  
	//이걸 모니터링 서버에서 복사하는데.. task별 on off 테이블을 따로 관리(이때는 task이름 사용)
	
	
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
