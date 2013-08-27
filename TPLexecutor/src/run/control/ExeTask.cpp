/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "comm/RemoteCLI.h"
#include "run/control/ExeTask.h"

ExeTask::ExeTask(RemoteCLI* svr, ExeTaskTreeNode* parent, std::string workName, int nameLine, std::string bhvname, RFSMtask* task, ModelBinder* tmPlugin, int exeType, int callstmtLine, unsigned int tp)
:ExeTaskTreeNode(parent, svr, workName, nameLine, task->getBlockID(), task->getFileName(), callstmtLine, tp)
{
	m_tmPlugin = tmPlugin;

	m_task				= task;
	m_startBehavior		= bhvname;
	m_behaviorTAB		= NULL;
	m_connTAB			= NULL;
//	m_outModelTAB		= new ValueMap();
	m_wVarTAB			= new ValueMap();
	m_monitorOff		= true;
	m_monCount			= 0;

	std::string logName("ExeTask-");
	logName.append(workName);

	if(TaskConfig::getIsTaskLog()){
		m_logFile = new LogFile((char*)(logName.c_str()), TaskConfig::getLogSize());
		m_logFile->setLevel( Log::LOG_LEVEL_DEBUG, true);
		m_logFile->setLevel( Log::LOG_LEVEL_TRACE, true);
		m_logFile->setLevel( Log::LOG_LEVEL_ERROR, true);
	}
	else{
		m_logFile = NULL;
	}

	pthread_cond_init(&m_SLEEPstmt_wakeup_cond, NULL);
	pthread_mutex_init(&m_SLEEPstmt_wakeup_mu, NULL);


	m_runner = new TaskRunner(this, tmPlugin, m_logFile);
	if(exeType == DEBUG){
		m_runner->setAsDebugMode();
	}
	else if(exeType == MONITOR){
		m_runner->setAsMonitorMode();
	}

	//�̰� ���⼭ �ʱ�ȭ �ϴ� ���� LogFile  �Ű����� ���� �����̴�.
	//if(m_tmPlugin->isSimul()) m_tmPlugin->initSimul(m_logFile);
	
	m_running = false;

#ifdef _WIN32
	m_TimerQueue = CreateTimerQueue();
#endif
}

ExeTask:: ~ExeTask(void)
{
	//m_taskTAB�� ������ �ܺο��� �����Ǵ� ���̱� ������ ���⼭ ����� �ȵȴ�.
	//m_piAdapter�� �ܺο��� �����Ǿ� ���޵� ���̶� �ܺο��� �����.. ���⼭ ����� �ȵȴ�.

	//if(m_startExeBehavior != NULL){
	//	delete m_startExeBehavior;
	//	m_startExeBehavior = NULL;
	//}

//	m_outModelTAB->clear();
//	delete m_outModelTAB;

	stopRunning();

	if(m_runner != NULL){ //���� ������ �߿���. m_logFile�� ���������Ǹ� �ȵ�.
		delete m_runner;
		m_runner = NULL;
	}

	if(m_wVarTAB != NULL){ 
		m_wVarTAB->clear();
		delete m_wVarTAB;
		m_wVarTAB = NULL;
	}

	if(m_logFile != NULL){ 
		delete m_logFile;
		m_logFile = NULL;
	}

	//ExeTaskTreeNode�� �Ҹ��ڿ��� ȣ������� m_missionNodes�� ���� ���� �����ϱ� ������
	//�ݵ�� childnode�� �����ϰ� m_mission�� �����ؾ� �ϱ� ����
	//(����)���� Ŭ������ �Ҹ��ڴ� ���߿� ȣ���
	//�̸� recall����� ������ �� ������ ����� �ϱ� ������ true�� ����
	removeAllChildNodes(true);
		
	if(m_missionNodes.size()>0){
		std::map<std::string, ExeTaskTreeNode*>::iterator mapiter;
		for(mapiter=m_missionNodes.begin(); mapiter!=m_missionNodes.end(); mapiter++){
			delete mapiter->second;
		}
	}
	m_missionNodes.clear();

	pthread_cond_destroy(&m_SLEEPstmt_wakeup_cond);
	pthread_mutex_destroy(&m_SLEEPstmt_wakeup_mu);

	//m_task�� TaskMem�� ������ �� ����
	//m_outModelTAB�� TaskMem���� �����Ѵ�.
}

TaskRunner* ExeTask::getController()
{
	return m_runner;
}

ExeTaskTreeNode* ExeTask::getMissionNode(std::string target)
{
	std::map<std::string, ExeTaskTreeNode*>::iterator mapiter;
	mapiter = m_missionNodes.find(target);
	if(mapiter != m_missionNodes.end()){
		return mapiter->second;
	}
	return NULL;
}

void ExeTask::printMsg(std::string msg)
{
	printf("[MSG] %s\n", msg.c_str());
}

std::string ExeTask::getStartBehaviorName()
{
	return m_startBehavior;
}

long ExeTask::getIterationCount()
{
	if(m_task != NULL) //execute2()
		return m_iterCount;
	//else //execute()
	//	return m_startExeBehavior->getIterCount();
}

void ExeTask::setBehaviorList(BehaviorTAB* bhvList)
{
	m_behaviorTAB = bhvList;
}

void ExeTask::setConnectorList(ConnectorTAB* connList)
{
	 m_connTAB = connList;
}

void ExeTask::setTaskSynchIds(std::set<int> sychIdsInTask)
{
	m_SychIdsInTask = sychIdsInTask;

	if(m_SychIdsInTask.size()>0){
		std::set<int>::iterator iter;
		for(iter=m_SychIdsInTask.begin();iter!=m_SychIdsInTask.end();iter++){
			int synchid = *iter;
			pthread_cond_t cond;
			pthread_cond_init(&cond, NULL);
			m_SynchIdCondVars.insert(std::pair<int, pthread_cond_t>(synchid, cond));

			pthread_cond_t cond2;
			pthread_cond_init(&cond2, NULL);
			m_SynchIdCondVars2.insert(std::pair<int, pthread_cond_t>(synchid, cond2));


			pthread_mutex_t mutex;
			pthread_mutex_init(&mutex, NULL);
			m_CondVarsMtxs.insert(std::pair<int, pthread_mutex_t>(synchid, mutex));
		}
	}
}

void ExeTask::setMissionNode(std::string target, ExeTaskTreeNode* node)
{
	m_missionNodes.insert(std::pair<std::string, ExeTaskTreeNode*>(target, node));
}

pthread_cond_t*	ExeTask::getSYNCHCond(int synchid)
{
	std::map<int, pthread_cond_t>::iterator	mapiter;
	mapiter = m_SynchIdCondVars.find(synchid);
	if(mapiter != m_SynchIdCondVars.end()){
		return &(mapiter->second);
	}
	return NULL;
}

pthread_cond_t*	ExeTask::getSYNCHCond2(int synchid)
{
	std::map<int, pthread_cond_t>::iterator	mapiter;
	mapiter = m_SynchIdCondVars2.find(synchid);
	if(mapiter != m_SynchIdCondVars2.end()){
		return &(mapiter->second);
	}
	return NULL;
}

pthread_mutex_t* ExeTask::getSYNCHMutex(int synchid)
{
	std::map<int, pthread_mutex_t>::iterator mapiter;
	mapiter = m_CondVarsMtxs.find(synchid);
	if(mapiter != m_CondVarsMtxs.end()){
		return &(mapiter->second);
	}
	return NULL;
}

pthread_mutex_t* ExeTask::getSLEEPStmtMutex()
{
	return &m_SLEEPstmt_wakeup_mu;
}

pthread_cond_t*	ExeTask::getSLEEPStmtCond()
{
	return &m_SLEEPstmt_wakeup_cond;
}

void ExeTask::releaseCond(void)
{
	std::map<int, pthread_cond_t>::iterator mapiter;
	for(mapiter = m_SynchIdCondVars2.begin(); mapiter!=m_SynchIdCondVars2.end(); mapiter++){
		pthread_cond_broadcast(&mapiter->second);
	}
	for(mapiter = m_SynchIdCondVars.begin(); mapiter!=m_SynchIdCondVars.end(); mapiter++){
		pthread_cond_broadcast(&mapiter->second);
	}
}

int ExeTask::waitingCntPlus(int synchid)
{
	pthread_mutex_lock(&m_lmu);
	int count = 1;
	m_mapiter = m_waitingCntMap.find(synchid);
	if(m_mapiter != m_waitingCntMap.end()){//map�� �̹� �����ϸ�
		count = m_mapiter->second;
		m_mapiter->second = ++count;
	}
	else{//map�� �������� ������
		m_waitingCntMap.insert(std::pair<int, int>(synchid, 1));
	}
printf("+(%d)%d\n", synchid, count);
	pthread_mutex_unlock(&m_lmu);
	return count;
}

void ExeTask::resetWaitingCnt()
{
	m_waitingCntMap.clear();
}

int ExeTask::waitingCntMinus(int synchid)
{
	pthread_mutex_lock(&m_lmu);
	int count = 0;
	m_mapiter = m_waitingCntMap.find(synchid);
	if(m_mapiter != m_waitingCntMap.end()){//map�� �̹� �����ϸ�
		count = m_mapiter->second;
		m_mapiter->second = --count;
	}
	else{//map�� �������� ������
		m_waitingCntMap.insert(std::pair<int, int>(synchid, 0));
	}
printf("-(%d)%d\n", synchid, count);
	pthread_mutex_unlock(&m_lmu);
	return count;
}

int ExeTask::getWaitingCnt(int synchid)
{
	pthread_mutex_lock(&m_lmu);
	int result = 0;
	m_mapiter = m_waitingCntMap.find(synchid);
	if(m_mapiter != m_waitingCntMap.end()){
		result =  m_mapiter->second;
	}
	pthread_mutex_unlock(&m_lmu);
	return result;
}

void ExeTask::lock_Synch(int synchid)
{
	pthread_mutex_lock(&m_lmu);
	m_lockedSynchIDs.insert(synchid);
	pthread_mutex_unlock(&m_lmu);
}

void ExeTask::unlock_Synch(int synchid)
{
	pthread_mutex_lock(&m_lmu);
	m_lockedSynchIDs.erase(synchid);
	pthread_mutex_unlock(&m_lmu);
}

bool ExeTask::isLocked_Synch(int synchid)
{
	pthread_mutex_lock(&m_lmu);
	bool result = false;
	std::set<int>::iterator it = m_lockedSynchIDs.find(synchid);
	if(it!=m_lockedSynchIDs.end())
		result = true;
	pthread_mutex_unlock(&m_lmu);
	return result;
}

/**
���� ����� ���� ����� SYNCH id�� ���¸� �𸣱� ������ 
root���� ���� �� ���¸� �˷��־�� �Ѵ�.
*/
void ExeTask::updateWaitingID()
{
	pthread_mutex_lock(&m_lmu);
	m_waitingIDs.clear();
	this->currentWaitingIDs(m_waitingIDs);
	pthread_mutex_unlock(&m_lmu);
}

bool ExeTask::isWaitingID(int synchid)
{
	pthread_mutex_lock(&m_lmu);
	bool result = false;
	if(m_waitingIDs.find(synchid)!=m_waitingIDs.end()){
		result = true;
	}
	pthread_mutex_unlock(&m_lmu);
	return result;
}

bool ExeTask::isAllSynchPassed(int synchid)
{
	pthread_mutex_lock(&m_lmu);
	bool result = false;
	if(getWaitingCnt(synchid)==0){
		result = true;
	}
	pthread_mutex_unlock(&m_lmu);
	return result;
}

void ExeTask::setWorkerVar(std::string varName, RFSMvalueEval valueEval)
{
	pthread_mutex_lock(&m_lmu);
	FLOG_TRACE(m_logFile, "  (RUNNING:WVAR, Write) %s, %s", varName.c_str(), valueEval.getStrValue().c_str());
	
	ValueMap::iterator it = m_wVarTAB->find(varName);
	if(it != m_wVarTAB->end()){
		RFSMvalueEval eval = it->second;
		//delete eval;
		m_wVarTAB->erase(it);
	}
	m_wVarTAB->insert(ValuePair(varName, valueEval));

	pthread_mutex_unlock(&m_lmu);
}

RFSMvalueEval ExeTask::getWorkerVar(std::string varName)
{
	pthread_mutex_lock(&m_lmu);

	ValueMap::iterator it = m_wVarTAB->find(varName);
	if(it != m_wVarTAB->end()){
		RFSMvalueEval eval = it->second;
		FLOG_TRACE(m_logFile, "  (RUNNING:WVAR, Read) %s, %s", varName.c_str(), eval.getStrValue().c_str());
	pthread_mutex_unlock(&m_lmu);		
		return eval;
	}
	else{
		FLOG_ERROR(m_logFile, "  (RUNNING:WVAR, Read) %s, Not initialized", varName.c_str());
	pthread_mutex_unlock(&m_lmu);		
		return RFSMvalueEval();
	}
}

RFSMtask* ExeTask::getTask()
{
	return m_task;
}

RFSMbehavior* ExeTask::getBehavior(std::string taskName)
{
	if(m_behaviorTAB != NULL){
		BehaviorTAB::iterator it = m_behaviorTAB->find(taskName);
		if(it != m_behaviorTAB->end()){
			return it->second;
		}
	}
	return NULL;
}

RFSMconnector* ExeTask::getConnector(std::string connName)
{
	if(m_connTAB != NULL){
		ConnectorTAB::iterator it = m_connTAB->find(connName);
		if(it != m_connTAB->end()){
			RFSMconnector* conn = it->second;
			return conn;
		}
	}
	return NULL;
}

int ExeTask::execute()
{	
	m_stop = false;
	m_running = true;
	m_joining = false;
	nodeStart();

	this->create_thr(RThread::ATTACH, getNodeName());
	if(m_runner->isDebugMode()||m_runner->isMonitorMode()){
		this->sendThreadInfo("new");
		m_runner->addCondMu4BP(getThreadNum());
		m_isDbgCondCreated = true;
	}

	return 0;
}

#ifdef _WIN32
void ExeTask::cbackBy_Timerfunc(PVOID param, bool timerOrWaitFired)
{

  	QueryPerformanceFrequency(&liFrequency);  // retrieves the frequency of the high-resolution performance counter    
	QueryPerformanceCounter(&liCounter1);         // Start
	litemp = (double)(liCounter1.QuadPart - liCounter0.QuadPart) / (double)liFrequency.QuadPart;
	liCounter0 = liCounter1;

	if(m_stop){
		SetEvent(m_DoneEvent);
		//m_running = false; //�̰� RThread���� �ִ� ��� ������
		return;
	}
		
	try{
		if(m_runner->isDebugMode() && !m_isDbgCondCreated){
			//������ε� cond�� ���� ������ �ȵǸ� �ƹ��͵� ���Ѵ�.
		}
		else{
			m_runner->enterTask(this, m_task);
			if(isCompleted()) {
				SetEvent(m_DoneEvent);
				return;
			}
		}
	}
	catch(RuntimeEH& runEH){
		//runEH.printError();
		FLOG_ERROR(m_logFile, "  (RUNNING:%d) ���� ȹ���ϴµ� �����Ͽ����ϴ�.", runEH.getErrorCode());
		//return runEH.getErrorCode();
	}

	FLOG_TRACE(m_logFile, "  (RUNNING:CYCLE) This is the end of a cycle.\n");
	m_iterCount++;			

	QueryPerformanceCounter(&liCounter2);
	PeRecord rec;
	rec.name = getInstanceName();
	rec.cycle = litemp;
	rec.proc = (double)(liCounter2.QuadPart - liCounter1.QuadPart) / (double)liFrequency.QuadPart;
	PerformTable::addPeRecord(rec);

	//printf("<%s> Cycle Time : %f, Process Time : %f\n", getInstanceName().c_str(), litemp, (double)(liCounter2.QuadPart - liCounter1.QuadPart) / (double)liFrequency.QuadPart);
	
}
#endif

void ExeTask::cbackBy_Thrfunc()
{
	bool isWin = false;
	m_iterCount = 0;
	m_running = true;
	m_joining = false;
	setCompleted(false);
	if(m_task != NULL){

		std::string performFile = getNodeName();
		if(TaskConfig::isloggingTimePeriod()) {
			PerformTable::init();			
		}
		#ifdef _WIN32	
		isWin = true;
		QueryPerformanceCounter(&liCounter0);  
		#else
		struct timespec tp0, tp1, tp2;
		double litemp;
		clock_gettime(CLOCK_MONOTONIC, &tp0);
		#endif

		if(isWin && TaskConfig::isUsingQueueTimer()){
			this->start_que(m_TimerQueue, getNodeName());
			if(TaskConfig::isloggingTimePeriod()) performFile.append("-Timer");	
		}
		else{
			//2. run
			while(1){
				
				#ifdef _WIN32	
				QueryPerformanceFrequency(&liFrequency);  // retrieves the frequency of the high-resolution performance counter    
				QueryPerformanceCounter(&liCounter1);         // Start
				litemp = (double)(liCounter1.QuadPart - liCounter0.QuadPart) / (double)liFrequency.QuadPart;
				liCounter0 = liCounter1;
				#else
				clock_gettime(CLOCK_MONOTONIC, &tp1);
				litemp = ((double)(tp1.tv_sec*1000) + (double)(tp1.tv_nsec)/1000000)-((double)(tp0.tv_sec*1000) + (double)(tp0.tv_nsec)/1000000);
				tp0.tv_sec = tp1.tv_sec;
				tp0.tv_nsec = tp1.tv_nsec;
				#endif

				if(m_stop){  //�ܺη� ���� ������ ���� ���			
					break;
				}
					
				try{
					if(m_runner->isDebugMode() && !m_isDbgCondCreated){
						//������ε� cond�� ���� ������ �ȵǸ� �ƹ��͵� ���Ѵ�.
					}
					else{
						m_runner->enterTask(this, m_task);
						if(isCompleted()) break;
					}
				}
				catch(RuntimeEH& runEH){
					//runEH.printError();
					FLOG_ERROR(m_logFile, "  (RUNNING:%d) ���� ȹ���ϴµ� �����Ͽ����ϴ�.", runEH.getErrorCode());
					//return runEH.getErrorCode();
				}
			
				FLOG_TRACE(m_logFile, "  (RUNNING:CYCLE) This is the end of a cycle.\n");
				m_iterCount++;	

				#ifdef _WIN32			
				QueryPerformanceCounter(&liCounter2);
				PeRecord rec;
				rec.name = getInstanceName();
				rec.cycle = litemp;
				rec.proc = (double)(liCounter2.QuadPart - liCounter1.QuadPart) / (double)liFrequency.QuadPart;				
				//printf("<%s> Cycle Time : %f, Process Time : %f\n", getInstanceName().c_str(), litemp, (double)(liCounter2.QuadPart - liCounter1.QuadPart) / (double)liFrequency.QuadPart);
				#else
				clock_gettime(CLOCK_MONOTONIC, &tp2);
				PeRecord rec;
				rec.name = getInstanceName();
				rec.cycle = litemp;
				rec.proc = ((double)(tp2.tv_sec*1000) + (double)(tp2.tv_nsec)/1000000)-((double)(tp1.tv_sec*1000) + (double)(tp1.tv_nsec)/1000000);				
				#endif

				if(TaskConfig::isloggingTimePeriod()){
					PerformTable::addPeRecord(rec);
				}
				
				float sleeptime = m_timePeriod - rec.proc*1000;
				if(sleeptime<0) sleeptime = 0;
				thread_sleep(sleeptime);
			
				//timespec a;
				//a.tv_sec = 0;
				//a.tv_nsec = 5000000 - rec.proc * 1000000;
				//nanosleep(&a, NULL);
				
			}
			if(TaskConfig::isloggingTimePeriod()) performFile.append("-Thread");	
		}
		m_runner->enterDestruct(this);
		m_running = false;
		
		if(TaskConfig::isloggingTimePeriod()) PerformTable::makeFile(performFile);
	}
	
	if(m_runner->isDebugMode()||m_runner->isMonitorMode()){
		m_runner->delCondMu4BP(getThreadNum());
		m_isDbgCondCreated = false;
		this->sendThreadInfo("delete"); //Task ������ delete�� ���۵Ǹ� �������� �� �� �ִ�.
		this->sendMonitorMsg("RES-EOT");
		DbgTable::clearTable();
	}
}

void ExeTask::breakThreadWait()
{
	//����� �� �� ������ ������ �����
	m_runner->wakeupAllBPwait();

	//����͸� �� ��
	m_runner->setResumeMonitor();

	//SLEEP�Լ� �����
	pthread_mutex_lock		(&m_SLEEPstmt_wakeup_mu);
	pthread_cond_broadcast	(&m_SLEEPstmt_wakeup_cond);
	pthread_mutex_unlock	(&m_SLEEPstmt_wakeup_mu);


	//SYNCH ��� ���� �����
	if(m_SychIdsInTask.size()>0){
		std::set<int>::iterator iter;
		for(iter=m_SychIdsInTask.begin();iter!=m_SychIdsInTask.end();iter++){
			int synchid = *iter;
			
			pthread_mutex_t*	mu	= getSYNCHMutex	(synchid);
			pthread_cond_t*		co	= getSYNCHCond	(synchid);
			pthread_cond_t*		co2 = getSYNCHCond2	(synchid);

			pthread_mutex_lock		(mu);
			pthread_cond_broadcast	(co);
			pthread_mutex_unlock	(mu);

			pthread_mutex_lock		(mu);
			removeWaitingID			(synchid);
			pthread_cond_broadcast	(co2);
			pthread_mutex_unlock	(mu);
		}
	}
}

int ExeTask::forceStop()
{
	m_stop = true; //stopRunning()������ �̺κ��� ���Ե����� breakThreadWait()�տ� m_stop=true�� �Ǿ�� �ϱ� ����
	breakThreadWait();
	this->stopRunning();

	//m_running = false;	
	while(isRunning()){
		thread_sleep(5);
	}
	//wait4join();					//ExeTaskRoot���� �̹� �ϰ� ����
	//m_runner->enterDestruct(this);	//run()���� �̺κ� ������
	return 0;
}

void ExeTask::initMonitorTAB()
{	
	MonitorTAB::iterator it = m_monTargetTAB.begin();
	for(; it!=m_monTargetTAB.end(); it++){
		MonitorRec mRec = it->second;
		mRec.on = false;
		mRec.after =  0;
		mRec.iter  = -1;
		mRec.siter =  0;
	}
	m_monCount = 0;
	//m_monitorAll = false;
}


void ExeTask::setCurTarget(std::string target)
{
pthread_mutex_lock(&m_lmu);
	m_curTarget = target;
pthread_mutex_unlock(&m_lmu);
}

std::string ExeTask::getCurTarget()
{
	return m_curTarget;
}

RFSMvalueEval ExeTask::getTaskTime(int line)
{
	if(m_task != NULL) //execute2() 
		return getNodeExeTime(line);
	//else //execute()
	//	return m_startExeBehavior->getNodeExeTime(line);
}



