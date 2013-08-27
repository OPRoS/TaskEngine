#include "comm/LocalCLI.h"

LocalCLI::LocalCLI(RemoteCLI* svr, std::ostream* ostr, OSFileSys* fsys, ModelBinder* binder)
{
//	TaskConfig::setMode(TaskConfig::OVERWRITE);
	m_ostr = ostr;
	m_fsys = fsys;

	m_binder = binder;

	m_taskMgr = new TaskManager((ModelBinder*)m_binder, svr, ostr);

	m_log =  Log("TASK-STATUS", ostr);
	m_log.setLevel(Log::LOG_LEVEL_ERROR, true);
	m_log.setLevel(Log::LOG_LEVEL_WARN, true);
	m_log.setLevel(Log::LOG_LEVEL_TRACE, true);

	//m_isInited = false;
	//m_isStarted = false;

//	m_simulMode = false;
//	if(m_simulMode)m_se = new SimulatedEnv();
//	else m_se = NULL;

//	m_standAloneMode = false;
	//if(m_standAloneMode)m_se = new SimulatedEnv();
	//else m_mb = NULL;
}

LocalCLI:: ~LocalCLI(void)
{
	if(m_taskMgr != NULL){
		delete m_taskMgr;
		m_taskMgr = NULL;
	}

	//delete m_binder; RemoteCLI에서 입력을 받는 경우 RemoteCLI에서 삭제.. 아니면 생성한 곳에서 삭제..(보통 할당받은 객체는 내부에서 삭제하면 안된다.)
	
//	if(m_binder != NULL)
//		delete m_binder;

//	if(m_se != NULL){
//		delete m_se;
//		m_se = NULL;
//	}
//	if(m_mb != NULL){
//		delete m_mb;
//		m_mb = NULL;
//	}

	//m_gVarTAB.clear();
}

/*
int TaskExecutor::init()
{
//	PO_CONNECT_FUNCTION(TaskExecutor, getWorkerList);
	OP_PUBLISH_FUNCTION(TaskExecutor, loadWorker);
	OP_PUBLISH_FUNCTION(TaskExecutor, unloadWorker);
	OP_PUBLISH_FUNCTION(TaskExecutor, startWorker);
	OP_PUBLISH_FUNCTION(TaskExecutor, stopWorker);
	OPUpdate();

	return 0;

}
*/

void LocalCLI::autoLoad()
{
	std::string depFile = TaskConfig::getDeployFile();
	loadWorker(depFile);
	std::string autoRunTask = TaskConfig::getAutoRunTask();
	if(autoRunTask!=""){
		startWorker(autoRunTask, NORMAL); //false는 디버깅모드가 아님을 의미
	}
}

/**
taskFileName이 "examples/helloworld/hw_task.tpl"의 형태로 입력된다.
이는 TASKHOME에서 부터 경로를 의미한다.
*/
int LocalCLI::loadWorker(std::string taskFileName)
{	
	if(m_taskMgr->isTaskRunning()){
		LOG_WARN(m_log, "Tasks are running now. Deployment of <%s> can not be performed.", taskFileName.c_str());
		showRunningTasks();	
		return RUNNING_TASK_EXIST;
	}
	int result = m_taskMgr->loadWorker(taskFileName, m_fsys);
	if(result ==0){
		LOG_TRACE(m_log, "Now, Every elements in the file '%s' are successfully loaded.", taskFileName.c_str());
		showDeployedTasks();
		//m_isInited = true;
		showDeployedBehaviors();
	}
	return result;
}

void LocalCLI::unloadWorker(std::string workName)
{
	int result = m_taskMgr->unloadWorker(workName);
	if(result == 0){
		LOG_TRACE(m_log,"Now, Task <%s> is successfully unloaded.", workName.c_str());
		showDeployedTasks();
		//m_isInited = false;
		showDeployedBehaviors();
	}
}

int LocalCLI::startWorker(std::string workName, int exeMode)
{
	//m_curWorker = workName;
	
//int result = m_taskMgr->startWorker(workName, asDebug);
	int result = m_taskMgr->startTask(workName, exeMode);
	if(result == 0){
		LOG_TRACE(m_log,"Now, Task %s is started.", workName.c_str());
		showRunningTasks();
		//m_isStarted = true;		
	}
/*
	else{
		int result2 = m_taskMgr->startWorker(workName, asDebug);
		if(result2 == 0){
			LOG_TRACE(m_log,"Now, Task %s is started.", workName.c_str());
			showRunningTasks();
			m_isStarted = true;	
			return result2;
		}
	}
*/
	return result;
}

int LocalCLI::stopWorker(std::string workName)
{
	ExeTask* t = m_taskMgr->getExeTask(workName);
	if(t != NULL){
		int eol = m_taskMgr->getTask(workName)->getEndLine();
		RFSMvalueEval eval = t->getTaskTime(eol);

		if(t != NULL)
			LOG_TRACE(m_log,"Task:%s, Running Time:%d, Iteration:%d", workName.c_str(), eval.getIntValue(), t->getIterationCount());
		
		int result = m_taskMgr->stopWorker(workName);
		if(result == 0){					
			LOG_TRACE(m_log,"Now, Task %s is stopped.", workName.c_str());
			showRunningTasks();
			//m_isStarted = true;
		}
		return result;
	}
	//m_isStarted = false;
	return -1;
}

void LocalCLI::stopAllWorker()
{
	int result = m_taskMgr->stopAllWorker();	
	if(result == 0){
		LOG_TRACE(m_log,"Now, All running tasks are stopped.");		
		showRunningTasks();
	}		
}

/*
void LocalCLI::procMonitorCmd(MonitorRequest monReq)
{
	int result = m_taskMgr->procMonitoring(monReq);
	if(result==0){
printf("Now, The monitoring request is successfully proccessed.\n");
		LOG_TRACE(m_log,"Now, The monitoring request is successfully proccessed.");
	}
	else{
		std::string taskName = monReq.getAttrValue(ATTR_task);
		LOG_ERROR(m_log, "TargetNotFoundException[%s]", taskName.c_str());
	}	
}
*/

ExeTask* LocalCLI::getExeTask(std::string tName)
{
	return m_taskMgr->getExeTask(tName);
}

TaskManager* LocalCLI::getTaskManager()
{
	return m_taskMgr;
}

void LocalCLI::list()
{
	showDeployedTasks();
	showDeployedBehaviors();
	showRunningTasks();
}

std::string LocalCLI::getRunningWorkerList()
{
	return m_taskMgr->getRunningWorkersList();
}

std::string LocalCLI::getLoadedWorkerList()
{
	return m_taskMgr->getLoadedWorkersList();
}

void LocalCLI::showRunningTasks()
{
	char* msg1 = "---------------------------<Running Task List>-------------------------------";
	char* msg2 = "-----------------------------------------------------------------------------";
	std::string runninglist = m_taskMgr->getRunningWorkersInfo();
	displayInfo(msg1, runninglist, msg2);
}

void LocalCLI::showDeployedTasks()
{
	char* msg1 = "----------------------------<Loaded Task List>-------------------------------";
	char* msg2 = "-----------------------------------------------------------------------------";
	std::string tasklist = m_taskMgr->getLoadedWorkersInfo();
	displayInfo(msg1, tasklist, msg2);
}

void LocalCLI::showDeployedBehaviors()
{
	char* msg1 = "--------------------------<Loaded Behavior List>-----------------------------";
	char* msg2 = "-----------------------------------------------------------------------------";
	std::string bhvlist = m_taskMgr->getBehaviorsInfo();
	std::string conlist = m_taskMgr->getConnectorsInfo();
	bhvlist.append(conlist);
	displayInfo(msg1, bhvlist, msg2);

}



void LocalCLI::displayInfo(char* head, std::string msg, char* end)
{
	std::string tasklist = m_taskMgr->getLoadedWorkersInfo();

	if(m_ostr != NULL)
		(*m_ostr)<<head<<"\r\n"<<msg<<"\r\n"<<end<<"\r\n";
	else{
		printf("%s\n", head);
		printf("%s\n", msg.c_str());
		printf("%s\n", end);
	}
}

/*
int TaskManagerPI::update()
{
	if(m_isInited && m_isStarted){
		//m_taskDB->updateWorker(m_curWorker);
		m_taskMgr->updateWorkers();
		return 1;
	}
	return 1;
}
*/


/*
void TaskManagerPI::setGlobalValue(LogFile* lf, std::string varName, RFSMvalueEval valueEval)
{
	FLOG_TRACE(lf, "  (RUNNING:GVar, Write) %s, %s", varName.c_str(), valueEval.getStrValue().c_str());

	ValueMap::iterator it = m_gVarTAB.find(varName);
	if(it != m_gVarTAB.end()){
		RFSMvalueEval eval = it->second;
		m_gVarTAB.erase(it);
	}
	m_gVarTAB.insert(ValuePair(varName, valueEval));
}

RFSMvalueEval TaskManagerPI::getGlobalValue(LogFile* lf, std::string name)
{
	ValueMap::iterator it = m_gVarTAB.find(name);
	if(it != m_gVarTAB.end()){
		RFSMvalueEval eval = it->second;
		FLOG_TRACE(lf, "  (RUNNING:GVar, Read) %s, %s", name.c_str(), eval.getStrValue().c_str());
		return eval;
	}
	else{
		FLOG_ERROR(lf, "  (RUNNING:GVar, Read) %s, Not initialized", name.c_str());
		return RFSMvalueEval();
	}
}

RFSMvalueEval TaskManagerPI::getModelValue(LogFile* lf, RFSMvariable* rvar)
{
	RFSMvalueEval eval;

	//POLog(logger::debug, "[PlugInAdapter] Getting PsychicPiranea.val");

	unsigned int type = rvar->getType();
	std::string name = rvar->getName();

	////////////////////////////////////////////////////////////////////
	if(m_simulMode){
		return m_se->getModelValue(lf, name);
	}
	///////////////////////////////////////////////////////////////////

	//AnyValue v;

	if(type == TYP_INT){

		try {
			//int intval = OPGetSymbolVal(name);
			//eval.setIntValue(intval);
			FLOG_TRACE(lf, "   (PLUG-IN:MODEL, IN) Type:int, name:%s, value:%d", name.c_str(), eval.getIntValue());
		} catch (...)
		{
			FLOG_ERROR(lf, "   (PLUG-IN:MODEL, IN) Type:int, name:%s, value:%d", name.c_str());
		}
	}
	if(type == TYP_FLOAT){
		try{
			//float f_val = OPGetSymbolVal(name);
			//eval.setFloatValue(f_val);
			FLOG_TRACE(lf, "   (PLUG-IN:MODEL, IN) Type:float, name:%s, value:%f", name.c_str(), eval.getFloatValue());
		} catch (...)
		{
			FLOG_ERROR(lf, "   (PLUG-IN:MODEL, IN) Type:float, name:%s, value:%f", name.c_str());
		}

	}
	if(type == TYP_STRING){
		try{
			//eval.setStrValue(OPGetSymbolVal(name));
			FLOG_TRACE(lf, "   (PLUG-IN:MODEL, IN) Type:string, name:%s, value:%s", name.c_str(), eval.getStrValue().c_str());
		} catch (...)
		{
			FLOG_ERROR(lf, "   (PLUG-IN:MODEL, IN) Type:string, name:%s, value:%s", name.c_str());
		}
	}

	return eval;

}

void TaskManagerPI::setModelValue(LogFile* lf, std::string name, RFSMvalueEval eval)
{
	unsigned int type = eval.getType();

	////////////////////////////////////////////////////////////////////
	if(m_simulMode){
		return m_se->setModelValue(lf, name, eval);
	}
	///////////////////////////////////////////////////////////////////

	//FLOG_TRACE(lf, "(PLUG-IN:MODEL, OUT) Setting model value");

	if(type == TYP_INT){
		try{
			//OPSetSymbolVal(name, AnyValue(eval.getIntValue()));
			FLOG_TRACE(lf, "   (PLUG-IN:MODEL, OUT) Type:int, name:%s , value:%s", name.c_str(), eval.getStrValue().c_str());
		} catch (...)
		{
			FLOG_ERROR(lf, "   (PLUG-IN:MODEL, OUT) Type:int, name:%s", name.c_str());
		}
		// LOG LOG LOG
	}
	if(type == TYP_FLOAT){
		try{
			//OPSetSymbolVal(name, AnyValue(eval.getFloatValue()));
			FLOG_TRACE(lf, "   (PLUG-IN:MODEL, OUT) Type:float, name:%s, value:%s", name.c_str(), eval.getStrValue().c_str());
		} catch (...)
		{
			FLOG_ERROR(lf, "   (PLUG-IN:MODEL, OUT) Type:float, name:%s", name.c_str());
		}
		// LOG LOG
	}
	if(type == TYP_STRING){
		try{
			//OPSetSymbolVal(name, AnyValue(eval.getStrValue()));
			FLOG_TRACE(lf, "   (PLUG-IN:MODEL, OUT) Type:string, name:%s, value:%s", name.c_str(), eval.getStrValue().c_str());
		} catch (...)
		{
			FLOG_ERROR(lf, "   (PLUG-IN:MODEL, OUT) Type:string, name:%s", name.c_str());
		}
	}
}


//   AnyList params;
//	params.push_back(AnyValue(1));
//	params.push_back(AnyValue(1.2));
//	POLog(logger::debug, "[Sample] Calling PsychicPiranea.getVal");
//	int i = OPCallFunction(&val"PsychicPiranea.getVal", params);


RFSMvalueEval TaskManagerPI::callFunction(LogFile* lf, int callType, unsigned int rtnType, std::string funcName, ValueMap* paramList)
{



	RFSMvalueEval rtnEval;

	/////////////////////////////////////////////////////////////////////
	if(m_simulMode){
		if(callType == RFSMcall::CALL_FUNC)
			return m_se->fucnctionCall(lf,rtnType, funcName, paramList);

		if(callType == RFSMcall::CALL_ACTION){
			m_se->actionCall(lf, funcName, paramList);
			return rtnEval;
		}
	}
	/////////////////////////////////////////////////////////////////////


	if(m_standAloneMode){
		m_mb->callFunction(rtnType, funcName, paramList);

	}

	/////////////////////////////////////////////////////////////////////

	//AnyValue val;

	if(paramList != NULL){
		//AnyList anyParams;
		std::map<std::string, RFSMvalueEval>::iterator it;
		for(it=paramList->begin() ; it!=paramList->end() ; it++){
			std::string pname = it->first;
			RFSMvalueEval eval = it->second;

			//입력 파라메터 변환
			if(eval.getType() == TYP_INT){
				//anyParams.push_back(AnyValue(eval.getIntValue()));
			}
			if(eval.getType() == TYP_FLOAT){
				//anyParams.push_back(AnyValue(eval.getFloatValue()));
			}
			if(eval.getType() == TYP_STRING){
				//anyParams.push_back(AnyValue(eval.getStrValue()));
			}
		}

		//함수호출

		if(rtnType == TYP_INT){
			try {
				//rtnEval.setIntValue(OPFunCall(funcName, anyParams));
			} catch (...)
			{
				FLOG_ERROR(lf, "   (PLUG-IN:FUNC/ACTION) returnType:int name:%s value:%s", funcName.c_str());
			}
		}
		if(rtnType == TYP_FLOAT){
			try {
				//rtnEval.setFloatValue(OPFunCall(funcName, anyParams));
			} catch (...)
			{
				FLOG_ERROR(lf, "   (PLUG-IN:FUNC/ACTION) returnType:float name:%s", funcName.c_str());
			}

		}
		if(rtnType == TYP_STRING){
			try {
				//rtnEval.setStrValue(OPFunCall(funcName, anyParams));
			} catch (...)
			{
				FLOG_ERROR(lf, "   (PLUG-IN:FUNC/ACTION) returnType:string name:%s", funcName.c_str());
			}
		}
		if(rtnType == TYP_VOID){
			try {
				//OPFunCall(funcName, anyParams);
			} catch (...)
			{
				FLOG_ERROR(lf, "   (PLUG-IN:FUNC/ACTION) returnType:string name:%s", funcName.c_str());
			}
		}

		FLOG_TRACE(lf, "   (PLUG-IN:FUNC/ACTION) Calling (function/Action):%s, return:%s", funcName.c_str(), rtnEval.getStrValue().c_str());
		return rtnEval;
	}
	else{

		if(rtnType == TYP_INT){
			try {
				//rtnEval.setIntValue(OPFunCall(funcName));
			} catch (...)
			{
				FLOG_ERROR(lf, "   (PLUG-IN:FUNC/ACTION) returnType:int name:%s", funcName.c_str());
			}
		}
		if(rtnType == TYP_FLOAT){
			try {
				//rtnEval.setFloatValue(OPFunCall(funcName));
			} catch (...)
			{
				FLOG_ERROR(lf, "   (PLUG-IN:FUNC/ACTION) returnType:float name:%s", funcName.c_str());
			}
		}
		if(rtnType == TYP_STRING){
			try {
				//rtnEval.setStrValue(OPFunCall(funcName));
			} catch (...)
			{
				FLOG_ERROR(lf, "   (PLUG-IN:FUNC/ACTION) returnType:string name:%s", funcName.c_str());
			}
		}
		if(rtnType == TYP_VOID){
			try {
				//OPFunCall(funcName);
			} catch (...)
			{
				FLOG_ERROR(lf, "   (PLUG-IN:FUNC/ACTION) returnType:string name:%s");
			}
		}
		FLOG_TRACE(lf, "   (PLUG-IN:FUNC/ACTION) Calling (function/Action):%s, return:%s", funcName.c_str(), rtnEval.getStrValue().c_str());
		return rtnEval;
	}

	return rtnEval;
}
*/

//void TaskManagerPI::initSimul(LogFile* lg)
//{
//	m_se->init(lg);
//}

//void TaskManagerPI::setAsSimulMode()
//{
//	m_simulMode = true;
//	m_se = new SimulatedEnv();
//}

//void TaskManagerPI::setAsStandAloneMode()
//{
//	m_standAloneMode = true;
//	m_mb = new DirectBinder();
//}

//bool TaskManagerPI::isSimul()
//{
//	return m_simulMode;
//}

/*
void TaskManagerPI::run()
{
	printf("update()\n");

	//pthread_mutex_t lmu = PTHREAD_MUTEX_INITIALIZER;
	//pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

	for(;;){
		//pthread_mutex_lock(&lmu);
		//pthread_cond_wait(&cond, &lmu);
		//if(m_isStarted)
			update();
		//else
			//break;
		//pthread_mutex_unlock(&lmu);
		//Sleep(10);
	}
}
*/
/*
extern "C"{
	void PrintError(char* msg, int status, char* fileName, int lineNumber) {
		std::cout << msg << ' ' << fileName << ":" << lineNumber
		<< " - " << strerror(status) << std::endl;
	}


	void* threadFunc(void* arg)
	{
		TaskManagerPI* pInst = static_cast<TaskManagerPI*>(arg);
		pInst->update();
		return NULL;
	}
}
*/

void LocalCLI::run()
{
	if(TaskConfig::isAutoDeploy()){
		autoLoad();
	}

	std::string input;
	std::string wname="";
	while(1){
		std::cout<<"load(0), unload(1), start(2), stop(3), exit(4)"<<std::endl;
		std::cout<<"select function[0-3]:";
		std::getline(std::cin, input);

		if(input.compare("0")== 0) {
			std::cout<<"Enter Task file name: ";
			std::cin>>wname;
			loadWorker(wname);
		}
		else if(input.compare("1") == 0) {
			std::cout<<"Enter Task name: ";
			std::cin>>wname;
			unloadWorker(wname);
		}
		else if(input.compare("2") == 0) {
			std::cout<<"Enter Task name: ";
			std::cin>>wname;
			startWorker(wname, NORMAL);
		}
		else if(input.compare("3") == 0) {
			std::cout<<"Enter Task name: ";
			std::cin>>wname;
			stopWorker(wname);
		}
		else if(input.compare("4") == 0){
			std::cout<<"Exit!"<<std::endl;
			break;
		}
		else{
			std::cout<<"It's invalid number."<<std::endl;
			std::cin.clear();
		}

		std::cin.ignore();
		thread_sleep(5);
	}
}

