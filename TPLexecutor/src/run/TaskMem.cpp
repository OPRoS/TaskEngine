/*
 * TaskMem.cpp
 *
 *  Created on: 2009. 3. 12
 *      Author: jinseo
 */

#include "run/TaskMem.h"

MonitorRec::MonitorRec()
{
	on = false; //생성될 때는 무조건 off로 간주한다.
}

MonitorRec::~MonitorRec()
{
}


TaskMem::TaskMem(std::ostream* ostr)
{
	m_log = Log("TASK-MEMORY", ostr);
	m_log.setLevel(Log::LOG_LEVEL_ERROR, true);
	m_log.setLevel(Log::LOG_LEVEL_INFO, true);
	m_log.setLevel(Log::LOG_LEVEL_TRACE, true);
	m_log.setLevel(Log::LOG_LEVEL_DEBUG, true);

//	m_taskTAB		= new StrMap();
	m_taskTAB2		= new TaskTAB();
	m_symTAB		= new SymTAB();
	m_behaviorTAB	= new BehaviorTAB();
	m_connectorTAB	= new ConnectorTAB();
	m_MonInfoTAB	= new MonitorTAB();

}

TaskMem:: ~TaskMem()
{
	if(m_symTAB !=NULL){
		SymTAB::iterator it;
		for(it=m_symTAB->begin() ; it!=m_symTAB->end() ; it++){
			//unloadWorker(it->first); //절대 delete it->second 하면 안된다.

			SymTAB4Task* symT = it->second;
			if(symT->getWorkerRefCount() <= 1){
				delete symT;
			}
			else{
				symT->minRefCount();
			}
		}
		m_symTAB->clear();
		delete m_symTAB;
		m_symTAB = NULL;
	}
/*
	if(m_taskTAB != NULL){
		m_taskTAB->clear();
		delete m_taskTAB;
		m_taskTAB = NULL;
	}
*/
	if(m_taskTAB2 !=NULL){
		TaskTAB::iterator it;
		for(it=m_taskTAB2->begin() ; it!=m_taskTAB2->end() ; it++){
			if(it->second != NULL) delete it->second;
		}
		m_taskTAB2->clear();
		delete m_taskTAB2;
		m_taskTAB2 = NULL;
	}

	if(m_behaviorTAB !=NULL){
		BehaviorTAB::iterator it;
		for(it=m_behaviorTAB->begin() ; it!=m_behaviorTAB->end() ; it++){
			if(it->second != NULL) delete it->second;
		}
		m_behaviorTAB->clear();
		delete m_behaviorTAB;
		m_behaviorTAB = NULL;
	}

	if(m_connectorTAB !=NULL){
		ConnectorTAB::iterator it;
		for(it=m_connectorTAB->begin() ; it!=m_connectorTAB->end() ; it++){
			delete it->second;
		}
		m_connectorTAB->clear();
		delete m_connectorTAB;
		m_connectorTAB = NULL;
	}


	if(m_MonInfoTAB !=NULL){
		//MonitorTAB::iterator it;
	//	for(it=m_MonInfoTAB->begin() ; it!=m_MonInfoTAB->end() ; it++){
	//		delete it->second;
	//	}
		m_MonInfoTAB->clear();
		delete m_MonInfoTAB;
		m_MonInfoTAB = NULL;
	}
}

int TaskMem::addMonitorInfo(RFSMbehavior* bhv)
{
	//대상 bhv에 대한 정보를 이미 만들었는지 검사
	std::string bname = bhv->getName();
	BehaviorTAB::iterator it = m_behaviorTAB->find(bname);
	if(it != m_behaviorTAB->end()){
		//이미 존재
		return 1;
	}	

	std::vector<RFSMstate*>* states = bhv->getAllStates();	
	if(states != NULL){
		for(unsigned int i=0 ; i<states->size(); i++){
			RFSMstate* state = (*states)[i];	
			std::string sname = state->getName();

			//key생성
			std::string key1 = sname+"."; //bhv.state
			std::string key2 = sname+".";
			std::string key3 = sname+".";
			std::string key4 = sname+".";

			key1.append(BLOCK_entry);
			key2.append(BLOCK_trans);
			key3.append(BLOCK_stay);
			key4.append(BLOCK_exit);

			//Monitor record 생성 및 저장
			MonitorRec mr1;// = new MonitorRec();
			m_MonInfoTAB->insert(MonRecPair(key1, mr1));
			MonitorRec mr2;// = new MonitorRec();
			m_MonInfoTAB->insert(MonRecPair(key2, mr2));
			MonitorRec mr3;// = new MonitorRec();
			m_MonInfoTAB->insert(MonRecPair(key3, mr3));
			MonitorRec mr4;// = new MonitorRec();
			m_MonInfoTAB->insert(MonRecPair(key4, mr4));
		}
	}
	
	//LOG_TRACE(m_log, "The symbol table for Task <%s> is newly added", workerName.c_str());
	return 0;
}

int TaskMem::addSymbolT(std::string workerName, SymTAB4Task* newSymT)
{
	//새로운 심벌태이블 등록
	SymTAB::iterator it = m_symTAB->find(workerName);

	if(it != m_symTAB->end()){//이미존재

		if(TaskConfig::getDeployMode() == TaskConfig::UNIQUE){
			LOG_ERROR(m_log, "The symbol table for the task, <%s>, already exists. In Unique-mode, it is a duplication error.", workerName.c_str());
			return DB_SYMTAB_ALREADY_EXIST_ERR;
		}
		if(TaskConfig::getDeployMode() == TaskConfig::ADDITION){
			LOG_DEBUG(m_log, "The symbol table of the task, <%s>, already exists. In Addition-mode, we skip to write the symbol table.", workerName.c_str());
			return 0;
		}

		if(TaskConfig::getDeployMode() == TaskConfig::OVERWRITE){
			SymTAB4Task* symT = it->second;

			if(symT != NULL && symT->getWorkerRefCount() <= 1){
				delete symT;
				it->second = NULL;
			}
			else{
				symT->minRefCount();
			}
			m_symTAB->erase(it);
			LOG_INFO(m_log, "The symbol table of the task <%s> already exists, In Overwrite-mode, we delete the exsiting symbol table.", workerName.c_str());
		}
	}

	typedef std::pair <std::string,SymTAB4Task*> valpair;
	m_symTAB->insert(valpair(workerName, newSymT));
	LOG_TRACE(m_log, "The symbol table of the task <%s> is newly added", workerName.c_str());
	return 0;
}

/*
int TaskMem::addTask(std::string workerName, std::string startBehaviorName)
{
	StrMap::iterator it = m_taskTAB->find(workerName);
	if(it != m_taskTAB->end()){

		if(TaskConfig::getDeployMode() == TaskConfig::UNIQUE){
			LOG_ERROR(m_log, "The input task <%s> already exists. In Unique-mode, it is a duplication error.", workerName.c_str());
			return DB_WORKER_ALREADY_EXIST_ERR;
		}
		if(TaskConfig::getDeployMode() == TaskConfig::ADDITION){
			LOG_INFO(m_log, "The input task <%s> already exists. In Addition-mode, we skip to write the input Worker.", workerName.c_str());
			return 0;
		}
		if(TaskConfig::getDeployMode() == TaskConfig::OVERWRITE){
			std::string startTask = it->second;
			m_taskTAB->erase(it);
			LOG_INFO(m_log, "The input task <%s> already exists. In Overwrite-mode, we delete the exsiting Worker", workerName.c_str());
		}
	}

	typedef std::pair <std::string, std::string> pair;
	m_taskTAB->insert(pair(workerName, startBehaviorName));
	LOG_TRACE(m_log, "The task <%s> is newly added", workerName.c_str());
	return 0;
}
*/
int TaskMem::addTask2(std::string tname, RFSMtask* rtask)
{
	TaskTAB::iterator it = m_taskTAB2->find(tname);
	if(it != m_taskTAB2->end()){

		if(TaskConfig::getDeployMode() == TaskConfig::UNIQUE){
			LOG_ERROR(m_log, "The input task <%s> already exists. In Unique-mode, it is a duplication error.", tname.c_str());
			return DB_WORKER_ALREADY_EXIST_ERR;
		}
		if(TaskConfig::getDeployMode() == TaskConfig::ADDITION){
			LOG_INFO(m_log, "The input task <%s> already exists. In Addition-mode, we skip to write the input Worker.", tname.c_str());
			return 0;
		}
		if(TaskConfig::getDeployMode() == TaskConfig::OVERWRITE){
			RFSMtask* t = it->second;
			delete t;
			m_taskTAB2->erase(it);
			LOG_INFO(m_log, "The input task <%s> already exists. In Overwrite-mode, we delete the exsiting Worker", tname.c_str());
		}
	}

	typedef std::pair <std::string, RFSMtask*> pair;
	m_taskTAB2->insert(pair(tname, rtask));
	LOG_TRACE(m_log, "The task, <%s>, is newly added", tname.c_str());
	return 0;
}

int TaskMem::addBehavior(RFSMbehavior* newBehavior)
{
	std::string newtName = newBehavior->getName();

	BehaviorTAB::iterator it = m_behaviorTAB->find(newtName);
	if(it != m_behaviorTAB->end()){
		if(TaskConfig::getDeployMode() == TaskConfig::UNIQUE){
			LOG_ERROR(m_log, "The behavior <%s> already exists. In Unique-mode, it is a duplication error.", newtName.c_str());
			return DB_TASK_ALREADY_EXIST_ERR;
		}
		if(TaskConfig::getDeployMode() == TaskConfig::ADDITION){
			LOG_INFO(m_log, "The behavior <%s> already exists. In addition-mode, we skip to write the exsiting Task.", newtName.c_str());
			return 0;
		}
		if(TaskConfig::getDeployMode() == TaskConfig::OVERWRITE){
			if(it->second != NULL){ 
				delete it->second;
			}
			m_behaviorTAB->erase(it);
			LOG_INFO(m_log, "The behavior <%s> already exists. In overwrite-mode, we delete the exsiting Task.", newtName.c_str());
		}

	}

	typedef std::pair <std::string, RFSMbehavior*> valpair;
	m_behaviorTAB->insert(valpair(newtName, newBehavior));
	LOG_TRACE(m_log, "The behavior <%s> is newly added", newtName.c_str());
	return 0;
}


int TaskMem::addConnector(RFSMconnector* newCon)
{
	std::string newtName = newCon->getName();

	ConnectorTAB::iterator it = m_connectorTAB->find(newtName);
	if(it != m_connectorTAB->end()){
		if(TaskConfig::getDeployMode() == TaskConfig::UNIQUE){
			LOG_ERROR(m_log, "The connector <%s> already exists. In Unique-mode, it is a duplication error.", newtName.c_str());
			return DB_TASK_ALREADY_EXIST_ERR;
		}
		if(TaskConfig::getDeployMode() == TaskConfig::ADDITION){
			LOG_INFO(m_log, "The connector <%s> already exists. In addition-mode, we skip to write the exsiting Task.", newtName.c_str());
			return 0;
		}
		if(TaskConfig::getDeployMode() == TaskConfig::OVERWRITE){
			RFSMconnector* pCon = it->second;
			delete pCon;
			m_connectorTAB->erase(it);
			LOG_INFO(m_log, "The connector <%s> already exists. In overwrite-mode, we delete the exsiting Task.", newtName.c_str());
		}

	}

	typedef std::pair <std::string, RFSMconnector*> valpair;
	m_connectorTAB->insert(valpair(newtName, newCon));
	LOG_TRACE(m_log, "The behavior <%s> is newly added", newtName.c_str());
	return 0;
}


RFSMbehavior* TaskMem::findBehavior(std::string behaviorName)
{
	//BehaviorTAB* behaviorTAB = m_taskMem->getBehaviorTAB();
	BehaviorTAB::iterator it2 = m_behaviorTAB->find(behaviorName);
	if(it2 != m_behaviorTAB->end()){
		return it2->second;
	}
	return NULL;
}

RFSMconnector* TaskMem::findConnector(std::string conName)
{
	//BehaviorTAB* behaviorTAB = m_taskMem->getBehaviorTAB();
	ConnectorTAB::iterator it2 = m_connectorTAB->find(conName);
	if(it2 != m_connectorTAB->end()){
		RFSMconnector* con = it2->second;
		return con;
	}
	return NULL;
}

/*
RFSMbehavior* TaskMem::getStartBehavior(std::string workName)
{
	//StrMap*	taskTAB = m_taskMem->getTaskTAB();
	std::map<std::string,  std::string>::iterator it = m_taskTAB->find(workName);
	if(it != m_taskTAB->end()){
		RFSMbehavior* task = findBehavior(it->second);
		if(task == NULL)
			LOG_ERROR(m_log, "The behavior ,<%s>, that is defined as a start point as the task ,<%s>, dose not exit.", it->second.c_str(), workName.c_str());
		return task;
	}
	return NULL;
}
*/

RFSMtask* TaskMem::getTask2(std::string workName)
{
	//StrMap*	taskTAB = m_taskMem->getTaskTAB();
	std::map<std::string,  RFSMtask*>::iterator it = m_taskTAB2->find(workName);
	if(it != m_taskTAB2->end()){
		RFSMtask* task = it->second;
		if(task == NULL)
			LOG_ERROR(m_log, "The task ,<%s>, dose not exit.", it->second->getName().c_str(), workName.c_str());
		return task;
	}	
	return NULL;
}


TABsymbol* TaskMem::getSymbol(std::string workerName, std::string name)
{
	//SymTAB* symTAB = m_taskMem->getSymbolTAB();
	SymTAB::iterator it = m_symTAB->find(workerName);
	if(it != m_symTAB->end()){//존재
		SymTAB4Task* symT = it->second;
		return symT->getSymbol(name);
	}
	return NULL;
}

/*
StrMap* TaskMem::getTaskTAB()
{
	return m_taskTAB;
}
*/
TaskTAB* TaskMem::getTaskTAB2()
{
	return m_taskTAB2;
}

SymTAB* TaskMem::getSymbolTAB()
{
	return m_symTAB;
}

BehaviorTAB* TaskMem::getBehaviorTAB()
{
	return m_behaviorTAB;
}

ConnectorTAB* TaskMem::getConnectorTAB()
{
	return m_connectorTAB;
}

MonitorTAB* TaskMem::getMonitorInfoTAB()
{
	return m_MonInfoTAB;
}

void TaskMem::leakTest()
{
	if(m_connectorTAB !=NULL){
		ConnectorTAB::iterator it;
		for(it=m_connectorTAB->begin() ; it!=m_connectorTAB->end() ; it++){
			delete it->second;
		}
		m_connectorTAB->clear();
		delete m_behaviorTAB;
		m_connectorTAB = NULL;
	}

	m_connectorTAB	= new ConnectorTAB();
}