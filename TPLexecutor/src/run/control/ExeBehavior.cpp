/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "run/control/ExeBehavior.h"

ExeBehavior::ExeBehavior(RemoteCLI* svr, LogFile* lf, ExeTaskTreeNode* parent, std::string name, int nameLine, std::vector<std::string> goalstates, std::string blockID, std::string filename, int runType, int callstmtLine, unsigned int tp)
:ExeTaskTreeNode(parent, svr, name, nameLine, blockID, filename, callstmtLine, tp)
{
	m_logFile = lf;
	//m_valueTAB = new varmap();

	m_goalStates = goalstates;
	//m_position = blockID;
	m_isCurStateEntered = false;
	m_isCurStateExited = false;
	//m_blockID = blockID;
	
	m_runType = runType;
	m_iterCount = 0;
}

ExeBehavior:: ~ExeBehavior(void)
{
	stopRunning();
	//if(m_outModelTAB !=NULL && m_outModelTAB->size() > 0){
	//	varmap::iterator it;
	//	for(it=m_outModelTAB->begin() ; it!=m_outModelTAB->end() ; it++){
	//		delete it->second;
	//	}
	//	m_outModelTAB->clear();
	//}
	m_goalStates.clear();
	m_params.clear();
}

void ExeBehavior::entryExecuted()
{
	m_isCurStateEntered = true;
}

void ExeBehavior::exitExecuted()
{
	m_isCurStateExited = true;
}

bool ExeBehavior::isEntryExecuted()
{
	return m_isCurStateEntered;
}

bool ExeBehavior::isExitExecuted()
{
	return m_isCurStateExited;
}

bool ExeBehavior::isOnGoalState()
{
	if(m_goalStates.size()>0){
		for(unsigned int i=0 ; i<m_goalStates.size(); i++){
			if(m_curState.compare(m_goalStates[i])==0)
				return true;
		}
	}
	return false;
}

void ExeBehavior::setCurState(std::string curState)
{
	m_curState = curState;
	m_isCurStateEntered = false;
	//m_isCurStateExited = false;
	stateStart();
}

void ExeBehavior::setCurStateBlock(unsigned int sb)
{
	m_curStateBlock = sb;
}

unsigned int ExeBehavior::getCurStateBlock()
{
	return m_curStateBlock;
}

std::string ExeBehavior::getCurState()
{
	return m_curState;
}

void ExeBehavior::stateStart()
{
	ftime(&m_curStateStart);

	//time_t timer= time(NULL);//current time
	//RFSMvalueEval stime = RFSMvalueEval();
	//stime.setIntValue((int)timer);
	//addLocalValue(STATE_TIME, stime);
}

RFSMvalueEval ExeBehavior::getStateTime(int line)
{
	struct timeb current;
	ftime(&current);	
	RFSMvalueEval stime(line, this->getNodeName());
	stime.setIntValue((current.time*1000+current.millitm)-(m_curStateStart.time*1000+m_curStateStart.millitm));
	return stime;

	//time_t timer= time(NULL);//current time
	//RFSMvalueEval stime =  getLocalValue(STATE_TIME);//start time
	//int timer2 = stime.getIntValue();
	//stime.setIntValue((int)timer-timer2);
	//return stime;
}

int ExeBehavior::execute()
{
	m_stop = false;
	setCompleted(false);
	std::string instName = getNodeName();
	instName.append(int2str(getCallStmtLine()));
	
	nodeStart();//BEHAVIOR_TIME
	this->create_thr(RThread::ATTACH, instName);
	if(m_exeTask->getController()->isDebugMode() ||m_exeTask->getController()->isMonitorMode()){
		this->sendThreadInfo("new");
		m_exeTask->getController()->addCondMu4BP(getThreadNum());
		m_isDbgCondCreated = true;
	}
	
	return 0;
}

void ExeBehavior::setCurParam(ValueMap params)
{
	m_params = params;
}


#ifdef _WIN32
//이 함수는 RThread::timerRoutine에서 호출된다.
void ExeBehavior::cbackBy_Timerfunc(PVOID param, bool timerOrWaitFired)
{
	QueryPerformanceFrequency(&liFrequency);  // retrieves the frequency of the high-resolution performance counter    
	QueryPerformanceCounter(&liCounter1);         // Start
	litemp = (double)(liCounter1.QuadPart - liCounter0.QuadPart) / (double)liFrequency.QuadPart;
	liCounter0 = liCounter1;

	TaskRunner* ctl = m_exeTask->getController();
	if(m_stop){//쓰레드 강제 중지 명령			
		SetEvent(m_DoneEvent);
		m_running = false;
		return;
	}
	try{
		if(ctl->isDebugMode() && !m_isDbgCondCreated){
			//디버깅인데 cond가 아직 생성이 안되면 아무것도 안한다.
		}
		else{
			ctl->enterBehavior(this, m_params);
			if(isOnGoalState() && isEntryExecuted()){			
				setCompleted(true);
				SetEvent(m_DoneEvent);
				m_running = false;
				return;
			}
		}
	}
	catch(RuntimeEH& runEH){
		//runEH.printError();
		FLOG_ERROR(m_logFile, "  (RUNNING:%d) 값을 획득하는데 실패하였습니다.", runEH.getErrorCode());
		//return runEH.getErrorCode();
	}

	QueryPerformanceCounter(&liCounter2);
	PeRecord rec;
	rec.name = getInstanceName();
	rec.cycle = litemp;
	rec.proc = (double)(liCounter2.QuadPart - liCounter1.QuadPart) / (double)liFrequency.QuadPart;
	PerformTable::addPeRecord(rec);
	//printf("<%s> Cycle Time : %f, Process Time : %f\n", getInstanceName().c_str(), litemp, (double)(liCounter2.QuadPart - liCounter1.QuadPart) / (double)liFrequency.QuadPart);
	
}
#endif

void ExeBehavior::cbackBy_Thrfunc()
{	
	bool isWin = false;
	m_running = true;
	m_joining = false;
	int wait = false;
	TaskRunner* ctl = m_exeTask->getController();

	#ifdef _WIN32	
	isWin = true;
	QueryPerformanceCounter(&liCounter0);  
	#else
	struct timespec tp0, tp1, tp2;
	float litemp;
	clock_gettime(CLOCK_MONOTONIC, &tp0);
	//t0 = ((tp0.tv_sec)*1000 + (double)(tp0.tv_nsec)/1000000 + 5);
	#endif

	if(isWin && TaskConfig::isUsingQueueTimer()){
		this->start_que(m_exeTask->getTimerQueue(), getNodeName());
	}
	else{
		while(1){
			#ifdef _WIN32	
			QueryPerformanceFrequency(&liFrequency);  // retrieves the frequency of the high-resolution performance counter    
			QueryPerformanceCounter(&liCounter1);         // Start
			litemp = (liCounter1.QuadPart - liCounter0.QuadPart) / (double)liFrequency.QuadPart;
			liCounter0 = liCounter1;
			#else
			clock_gettime(CLOCK_MONOTONIC, &tp1);
			//t1 = ((tp1.tv_sec)*1000 + (double)(tp1.tv_nsec)/1000000 + 5);
			litemp = ((double)(tp1.tv_sec*1000) + (double)(tp1.tv_nsec)/1000000)-((double)(tp0.tv_sec*1000) + (double)(tp0.tv_nsec)/1000000);
			tp0 = tp1;
			#endif

			if(m_stop)//쓰레드 강제 중지 명령			
				break;
			try{
				if(ctl->isDebugMode() && !m_isDbgCondCreated){
					//디버깅인데 cond가 아직 생성이 안되면 아무것도 안한다.
				}
				else{
					ctl->enterBehavior(this, m_params);
					if(isOnGoalState() && isEntryExecuted()){			
						setCompleted(true);
						break;
					}
				}
			}
			catch(RuntimeEH& runEH){
				//runEH.printError();
				FLOG_ERROR(m_logFile, "  (RUNNING:%d) 값을 획득하는데 실패하였습니다.", runEH.getErrorCode());
				//return runEH.getErrorCode();
			}

			#ifdef _WIN32			
			QueryPerformanceCounter(&liCounter2);
			PeRecord rec;
			rec.name = getInstanceName();
			rec.cycle = litemp;
			rec.proc = (double)(liCounter2.QuadPart - liCounter1.QuadPart) / (double)liFrequency.QuadPart;
			//printf("<%s> Cycle Time : %f, Process Time : %f\n", getInstanceName().c_str(), litemp, (double)(liCounter2.QuadPart - liCounter1.QuadPart) / (double)liFrequency.QuadPart);
			#else
			clock_gettime(CLOCK_MONOTONIC, &tp2);
			//t2 = ((tp2.tv_sec)*1000 + (double)(tp2.tv_nsec)/1000000 + 5);
			PeRecord rec;
			rec.name = getInstanceName();
			rec.cycle = litemp;
			rec.proc = ((double)(tp2.tv_sec*1000) + (double)(tp2.tv_nsec+5)/1000000)-((double)(tp1.tv_sec*1000) + (double)(tp1.tv_nsec)/1000000);			
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
	}
	m_exeTask->getController()->enterDestruct(this);
	m_running = false;
	if(ctl->isDebugMode()||ctl->isMonitorMode()){
		ctl->delCondMu4BP(getThreadNum());
		m_isDbgCondCreated = false;
		this->sendThreadInfo("delete");
	}
}

int ExeBehavior::resume()
{	
	m_isCurStateEntered = false;
	m_isCurStateExited = false;
	execute();
	return 0;
}

long ExeBehavior::getIterCount()
{
	return m_iterCount;
}
