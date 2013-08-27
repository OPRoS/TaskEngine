/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "comm/RemoteCLI.h"
#include "run/control/ExeTaskTreeNode.h"

ExeTaskTreeNode::ExeTaskTreeNode(ExeTaskTreeNode* parent, RemoteCLI* svr, std::string nodeName, int nodeNameLine, std::string blockid, std::string fileName, int callstmtLine, unsigned int tp): RThread("ExeTaskTreeNode", tp)
{	
	m_runType				= NOTHREAD;	
	m_completed				= false;
	m_isConstructed			= false;
	m_isDestructed			= false;
	m_svr					= svr;		//���⼭ delete�ϸ� �ȵ�
	m_parent				= parent;	//���⼭ delete�ϸ� �ȵ�	
	m_monRes				= new MonitorResponse();
	m_nodeName				= nodeName;
	m_nodeNameLine			= nodeNameLine;
	m_callstmtLine			= callstmtLine;
	m_blockID				= blockid;
	m_fileName				= fileName;
	m_isMission				= false;
	m_isActBlockSelected	= false;
	m_enterAfterConstBL		= false;
	m_isDbgCondCreated		= false;
	m_joining				= false;
	m_nextTarget			= "";

	pthread_mutex_init(&m_lmu2, NULL);
}

ExeTaskTreeNode::~ExeTaskTreeNode(void)
{
	removeAllChildNodes(true); //�̸� recall����� ������ �� ������ ����� �ϱ� ������ true�� ����

	if(m_monRes != NULL){ 
		delete m_monRes;
		m_monRes = NULL;
	}	
	//m_logFile�� ���⼭ ����� �ȵȴ�.

	m_valueTAB.clear();
	m_WaitingSynchIDs.clear();

	//while(isRunning()) thread_sleep(5);
	pthread_mutex_unlock(&m_lmu2);
	pthread_mutex_destroy(&m_lmu2);
}

void ExeTaskTreeNode::addChild(ExeTaskTreeNode* child)
{
	m_childNodes.push_back(child);
}

void ExeTaskTreeNode::setExeTask(ExeTask* eTask)
{
	m_exeTask = eTask;
}

void ExeTaskTreeNode::setParent(ExeTaskTreeNode* parent)
{
	m_parent = parent;
}

void ExeTaskTreeNode::enterAfterConstBL(bool val)
{
	m_enterAfterConstBL = val;
}

bool ExeTaskTreeNode::isRunnedAfterConstBL()
{
	if(m_childNodes.size()>0){
		for(unsigned int i=0 ; i<m_childNodes.size(); i++){
			if(m_childNodes[i] != NULL && typeid(*m_childNodes[i])==typeid(ExeConnector)){
				bool result = (ExeConnector*)m_childNodes[i]->isRunnedAfterConstBL();
				if(!result) return false;
			}
		}
	}
	return m_enterAfterConstBL;
}

void ExeTaskTreeNode::removeChildNode(std::string name, bool isTaskEnd, int line)
{
	if(m_childNodes.size()>0){
		for(unsigned int i=0; i<m_childNodes.size();i++){
			if(m_childNodes[i] != NULL){
				std::string cName = m_childNodes[i]->getNodeName();
				int cline = m_childNodes[i]->getCallStmtLine();
				if(name.compare(cName)==0 && line==cline){
					//1. m_childNodes[i]�� �ڽĳ����� ���� ����
					if(m_childNodes[i]->isRunning()) m_childNodes[i]->stopRunning();
					m_childNodes[i]->removeAllChildNodes(isTaskEnd);
					
					//if(m_childNodes[i]->isMission() && !m_childNodes[i]->isCompleted() && !isTaskEnd){
					//2. m_childNodes[i] ����
					if(m_childNodes[i]->isMission()){
						m_childNodes[i]->setParent(NULL);
						//�̼ǳ���� ���� �������.(��, �Ϸᰡ �ƴϰ�, ���� �½�ũ�� ������ �� ��Ȳ�� �ƴ� ����)
						//m_childNodes.erase(m_childNodes.begin() + i); �̰� �Ʒ����� �Ѵ�.
					}
					else if(m_childNodes[i] != NULL) delete m_childNodes[i];
					
					m_childNodes[i] = NULL;
					m_childNodes.erase(m_childNodes.begin()+i);
					break;
				}
			}
		}        //goal state�� ����� �극��ũ ������ �׽�Ʈ �� ��
	}
}

void ExeTaskTreeNode::removeAllChildNodes(bool isTaskEnd)
{
	if(m_childNodes.size()>0){
		for(unsigned int i=0 ; i<m_childNodes.size(); i++){
			if(m_childNodes[i] != NULL){
				if(m_childNodes[i]->isRunning()) m_childNodes[i]->stopRunning();
				m_childNodes[i]->removeAllChildNodes(isTaskEnd);
			
				//if(m_childNodes[i]->isMission() && !m_childNodes[i]->isCompleted() && !isTaskEnd){
				if(m_childNodes[i]->isMission()){
					m_childNodes[i]->setParent(NULL);
					m_childNodes.erase(m_childNodes.begin() + i);
					--i;
				}
				else{
					if(m_childNodes[i] != NULL){
						delete m_childNodes[i];
						m_childNodes[i] = NULL;
					}
				}
				
				//m_childNodes.erase(m_childNodes.begin()+i); �̰� �ϸ� �ȵ�
			}
		}
		m_childNodes.clear();
	}
}

bool ExeTaskTreeNode::isChildExist(std::string name, int line)
{
	if(m_childNodes.size()>0){
		for(unsigned int i=0;i<m_childNodes.size();i++){	
			std::string cName = m_childNodes[i]->getNodeName();
			int cline = m_childNodes[i]->getCallStmtLine();
			if(name.compare(cName)==0 && line==cline)
				return true;
		}
	}
	return false;
}

ExeTaskTreeNode* ExeTaskTreeNode::getChildNode(std::string name, int line)
{
	if(m_childNodes.size()>0){
		for(unsigned int i=0;i<m_childNodes.size();i++){			
			std::string cName = m_childNodes[i]->getNodeName();
			int cline = m_childNodes[i]->getCallStmtLine();
			if(name.compare(cName)==0 && line==cline)
				return m_childNodes[i];
		}
	}
	
	return NULL;
}

std::vector<ExeTaskTreeNode*> ExeTaskTreeNode::getAllChildNodes()
{
	return m_childNodes;
}

std::vector<ExeTaskTreeNode*>* ExeTaskTreeNode::getAllChildNodesP()
{
	return &m_childNodes;
}

int ExeTaskTreeNode::getNodeNameLine()
{
	return m_nodeNameLine;
}

int	ExeTaskTreeNode::getCallStmtLine(void)
{
	return m_callstmtLine;
}

std::string ExeTaskTreeNode::getNodeName()
{
	return m_nodeName;
}

std::string ExeTaskTreeNode::getFileName()
{
	return m_fileName;
}


std::string ExeTaskTreeNode::getBlockID()
{
	return m_blockID;
}


void ExeTaskTreeNode::addLocalValue(std::string varName, RFSMvalueEval valueEval)
{
//#ifdef	__DEBUG_WORKER_RUNNING_
//printf("[MDG] (RUNNING:Var, Write) %s, %s\n", varName.c_str(), valueEval.getStrValue().c_str());
//#endif
pthread_mutex_lock(&m_lmu);
	FLOG_TRACE(m_logFile, "   (RUNNING:LocalVar, Write) %s, %s", varName.c_str(), valueEval.getStrValue().c_str());

	int originLine;
	std::string originTarget;

	ValueMap::iterator it = m_valueTAB.find(varName);
	if(it != m_valueTAB.end()){//�̹� ����� ������ ���� ���Ӱ� �Ҵ��ϴ� ���
		RFSMvalueEval eval = it->second;
		originLine = eval.getLine();
		originTarget = eval.getTarget();
		valueEval.setLine(originLine);
		valueEval.setTarget(originTarget);
		//delete eval;
		m_valueTAB.erase(it);
	}

	m_valueTAB.insert(ValuePair(varName, valueEval));
	//m_valueLineTAB.insert(ValueLinePair(varName, line));
pthread_mutex_unlock(&m_lmu);
}

RFSMvalueEval ExeTaskTreeNode::getLocalValue(std::string name)
{
pthread_mutex_lock(&m_lmu);
	ValueMap::iterator it = m_valueTAB.find(name);
	if(it != m_valueTAB.end()){
		RFSMvalueEval eval = it->second;

//#ifdef	__DEBUG_WORKER_RUNNING_
//printf("[MDG] (RUNNING:Var, Read) %s, %s\n", name.c_str(), eval.getStrValue().c_str());
//#endif
		FLOG_TRACE(m_logFile, "   (RUNNING:LocalVar, Read) %s, %s", name.c_str(), eval.getStrValue().c_str());
pthread_mutex_unlock(&m_lmu);
		return eval;
	}
	else{
pthread_mutex_unlock(&m_lmu);
		return RFSMvalueEval();
	}
}

void ExeTaskTreeNode::nodeStart()
{	
	ftime(&m_nodeStart);

	//time_t timer= time(NULL);
	//RFSMvalueEval ttime = RFSMvalueEval();
	//ttime.setIntValue((int)timer);
	//addLocalValue(BEHAVIOR_TIME, ttime);
}

RFSMvalueEval ExeTaskTreeNode::getNodeExeTime(int line)
{
	struct timeb current;
	ftime(&current);	
	RFSMvalueEval ttime(line, this->getNodeName());
	ttime.setIntValue((current.time*1000+current.millitm)-(m_nodeStart.time*1000+m_nodeStart.millitm));
	return ttime;
   
	//time_t timer= time(NULL);//����ð�
	//RFSMvalueEval ttime = getLocalValue(BEHAVIOR_TIME);
	//int timer2 = ttime.getIntValue();//���۽ð�
	//ttime.setIntValue((int)timer-timer2);
	//return ttime;
}

/*
int ExeTaskTreeNode::stop()
{
	m_stop = true;
	//1.�ڽĺ��� cancel()
	if(m_childNodes.size()>0){
		for(unsigned int i=0; i<m_childNodes.size();i++){
			if(m_running)
				m_childNodes[i]->stop();
		}
	}
	//2.�ڽ��� ������ cancel
	cancel();
	m_running = false;
	//cleanUp();
	return 0;
}
*/

void ExeTaskTreeNode::setRunType(int type)
{
	m_runType = type;
}

int ExeTaskTreeNode::getRunType()
{
	return m_runType;
}

void ExeTaskTreeNode::setStopFlag(void)
{
	m_stop = true;
	if(m_childNodes.size()>0){
		for(unsigned int i=0; i<m_childNodes.size();i++){			
			m_childNodes[i]->setStopFlag();
		}
	}
}

int ExeTaskTreeNode::stopRunning()
{
	pthread_mutex_lock(&m_lmu2);	
	if(m_joining){
		pthread_mutex_unlock(&m_lmu2); 
		return -1;
	}
	
	setStopFlag();

	m_joining	= true;
	if(m_childNodes.size()>0){
		for(unsigned int i=0; i<m_childNodes.size();i++){			
			if(!m_childNodes[i]->isJoining()) m_childNodes[i]->stopRunning();
		}
	}

	if(typeid(*this)==typeid(ExeConnector)){
		//connector��� ��ü�� ������� ����Ǵ°� �ƴϴ�.
		m_running = false; 
	}
	else if(m_runType==SYNCHRO || m_runType==ASYNCHRO) {
		if(m_running) wait4join();
		m_running = false;
	}
	else if(m_runType=NOTHREAD){//�����尡 �ƴ� ExeBehavior 
		m_running = false;
	}
	m_joining = false;

	//if(typeid(*this)==typeid(ExeTask)){
	//	((ExeTask*)this)->releaseCond();
	//}
	pthread_mutex_unlock(&m_lmu2);
	return 0;
}


bool ExeTaskTreeNode::isCompleted()
{
	return m_completed;
}

void ExeTaskTreeNode::setCompleted(bool val)
{
printf("%s: completed\n", this->getNodeName().c_str());
	m_completed = val;
}


/*
bool ExeTaskTreeNode::isSynched4Loop()
{
	if(m_childNodes.size()>0){
		for(unsigned int i=0; i<m_childNodes.size();i++){
			if(m_childNodes[i] != NULL){
				if(!m_childNodes[i]->isSynched4Loop()){
//printf("%s is unsynched\n", m_name.c_str());
					return false;
				}
			}
		}
//printf("%s is synched\n", m_name.c_str());
		
		
		return true;
	}

	//�ܸ������ ���
	return m_loopSynched;
	

//if(m_loopSynched)printf("%s is synched\n", m_name.c_str());
//else printf("%s is unsynched\n", m_name.c_str());
	
	//return m_loopSynched;
}


void ExeTaskTreeNode::setSynched4Loop()
{
//printf("synched: %s\n", m_name.c_str());
	m_loopSynched = true;
	if(m_childNodes.size()>0){
		for(unsigned int i=0; i<m_childNodes.size();i++){
			if(m_childNodes[i] != NULL) 
				m_childNodes[i]->setSynched4Loop();
		}
	}
}

void ExeTaskTreeNode::setUnSynched4Loop()
{
//printf("unsynched: %s\n", m_name.c_str());
	m_loopSynched = false;
//	if(m_childNodes.size()>0){
//		for(unsigned int i=0; i<m_childNodes.size();i++){
//			if(m_childNodes[i] != NULL) 
//				m_childNodes[i]->setUnSynched4Loop();
//		}
//	}
}


void ExeTaskTreeNode::setEOI(bool val)
{
	m_isEOI = val;
}

bool ExeTaskTreeNode::isEOI()
{
	if(m_childNodes.size()>0){
		for(unsigned int i=0; i<m_childNodes.size();i++){
			if(m_childNodes[i] != NULL) 
				if(!m_childNodes[i]->isEOI())
					return false;
		}
	}

	return m_isEOI;
}

*/

ExeTaskTreeNode* ExeTaskTreeNode::getConnectorParent()
{
	if(typeid(*this)==typeid(ExeConnector)){
		return this;
	}
	else if(m_parent == NULL){
		return NULL;
	}
	else{
		return m_parent->getConnectorParent();
	}
}

/**
���Լ��� �ڽĳ�忡�� �θ���� ȣ��ȴ�.
*/
void ExeTaskTreeNode::notifyWaitingID(int id)
{
	pthread_mutex_lock(&m_lmu);

	if(m_parent == NULL){
		//root
	}
	else if(typeid(*this)==typeid(ExeConnector)){
		//num: ������忡�� ��� �ϴ� SYNCH id�� ���� �����ϸ� �� �Լ��� ȣ���Ѵ�.
		int num = 0;
		m_mapIter = m_WaitingSynchIDs.find(id);
		if(m_mapIter != m_WaitingSynchIDs.end()){//map�� �̹� �����ϸ�
			int count = m_mapIter->second;
			num = ++count;
			m_mapIter->second = num;
		}
		else{//map�� �������� ������
			m_WaitingSynchIDs.insert(std::pair<int, int>(id, 1));
			num = 1;
		}
		//num2: ���� ��忡�� ����ؾ��ϴ� SYNCH id�� ��[getSynchIDNum(id)];
		int num2 = ((ExeConnector*)this)->getSynchIDNum(id);
		
		if(num == num2)//���� ��忡�� ����ϴ� SYNCH id���� ����
			m_parent->notifyWaitingID(id);

	}
	else if(typeid(*this)==typeid(ExeBehavior)){
		//Behavior�� ������ ���� ���� ����
		m_parent->notifyWaitingID(id);
	}

	pthread_mutex_unlock(&m_lmu);
}

void ExeTaskTreeNode::removeWaitingID(int synchid)
{
	pthread_mutex_lock(&m_lmu);
	m_WaitingSynchIDs.erase(synchid);
	if(m_childNodes.size()>0){
		for(unsigned int i=0; i<m_childNodes.size();i++){
			if(m_childNodes[i] != NULL) 
				m_childNodes[i]->removeWaitingID(synchid);
		}
	}
	pthread_mutex_unlock(&m_lmu);
}

/**
���Լ��� �θ��忡�� �ڽĳ��� ȣ��ȴ�.
*/
void ExeTaskTreeNode::currentWaitingIDs(std::set<int>& ids)
{
	pthread_mutex_lock(&m_lmu);

	if(typeid(*this)==typeid(ExeConnector)){
		std::map<int, int>::iterator mapiter;
		for(mapiter=m_WaitingSynchIDs.begin();mapiter!=m_WaitingSynchIDs.end();mapiter++){
			int id = mapiter->first;
			int num = mapiter->second;
			int num2 = ((ExeConnector*)this)->getSynchIDNum(id);
			if(num!=num2) ids.insert(id);
		}
	}

	if(m_childNodes.size()>0){
		for(unsigned int i=0; i<m_childNodes.size();i++){
			m_childNodes[i]->currentWaitingIDs(ids);
		}
	}

	pthread_mutex_unlock(&m_lmu);
}

void ExeTaskTreeNode::setActBlockSelected(bool val)
{
	if(val)
		m_isActBlockSelected = true;
	else{
		m_isActBlockSelected = false;
		if(m_childNodes.size()>0){
			for(unsigned int i=0; i<m_childNodes.size();i++){
				if(m_childNodes[i] != NULL) 
					m_childNodes[i]->setActBlockSelected(false);
			}
		}
	}
}

void ExeTaskTreeNode::setActBlockExecuted(bool val)
{
	if(val)
		m_isActBlockExecuted = true;
	else{
		m_isActBlockExecuted = false;
		if(m_childNodes.size()>0){
			for(unsigned int i=0; i<m_childNodes.size();i++){
				if(m_childNodes[i] != NULL) 
					m_childNodes[i]->setActBlockExecuted(false);
			}
		}
	}
}

bool ExeTaskTreeNode::isActBlockSelected()
{
	if(m_childNodes.size()>1){
		for(unsigned int i=0; i<m_childNodes.size();i++){
			if(m_childNodes[i] != NULL) 
				if(!m_childNodes[i]->isActBlockSelected())
					return false;
		}
		m_isActBlockSelected = true;
	}
	else if(m_childNodes.size()==1){
		m_isActBlockSelected = m_childNodes[0]->isActBlockSelected();
	}
	
	return m_isActBlockSelected;
}

bool ExeTaskTreeNode::isActBlockExecuted()
{
	if(m_childNodes.size()>1){
		for(unsigned int i=0; i<m_childNodes.size();i++){
			if(m_childNodes[i] != NULL) 
				if(!m_childNodes[i]->isActBlockExecuted())
					return false;
		}
		m_isActBlockExecuted = true;
	}
	else if(m_childNodes.size()==1){
		m_isActBlockExecuted = m_childNodes[0]->isActBlockExecuted();
	}
	
	return m_isActBlockExecuted;
}

unsigned int ExeTaskTreeNode::getChildNum()
{
	return m_childNodes.size();
}

bool ExeTaskTreeNode::isStopped()
{
	return m_stop;
}

/**
  @line
  var�� null�̸� blockpath�� ���� �����ؾ� �Ѵ�.
*/
void ExeTaskTreeNode::sendDebugMsg(ValueMap* modelValTab, std::string taskname, int line, std::string blockpath)
{
	std::string bpath = blockpath;
	std::string dbgmsg = "";
	std::vector<std::string> path = Estring::split(bpath, ".");

	for(unsigned int k=path.size(); k>0; k--){
		std::string c_path="";
		for(unsigned int i=0; i<k; i++){//���� ����� ��� ���� ���
			c_path.append(path.at(i));
			if(i+1<k)c_path.append(".");
		}

		ValueMap::iterator it;
		for(it = m_valueTAB.begin(); it!=m_valueTAB.end();it++){
			std::string	vkeyname = it->first;
			size_t found = vkeyname.find_last_of(".");
			std::string vblock = vkeyname.substr(0, found);
			std::string vname = vkeyname.substr(found+1, vkeyname.size());
			if (c_path.compare(vblock)==0){ //��������� ������ ��ϰ� ���ٸ�(��, ���� ����� ����)
				RFSMvalueEval value = it->second;
				if(value.getLine()<=line){
					dbgmsg.append(makeDebugMsg(taskname, this->getFileName(), line, vname, value));//+2�� 0.k���� ��(.)�ǳʱ�
					//sendMonitorMsg(dbgmsg);
				}
			}
		}
	}

	if(dbgmsg == ""){
		dbgmsg.append(makeEmptyDebugMsg(taskname, this->getFileName(), line));
	}

	/*
	//���⿡ model���̺� ���� ����� ������ �߰�
	ValueMap::iterator it2;
	for(it2 = modelValTab->begin(); it2!=modelValTab->end(); it2++){
		std::string	mname = it2->first;
		RFSMvalueEval mvalue = it2->second;
		dbgmsg.append(makeDebugMsg(taskname, "model", line, mname, mvalue));
	}
	*/


	dbgmsg.append("RES-EOB");
	sendMonitorMsg(dbgmsg);
}

/*
void ExeTaskTreeNode::sendDebugMsg(int line, RFSMvariable* var, std::string blockpath)
{
	std::string bpath = "";
	if(var != NULL){//���� ����� ������ var�� ���
		bpath = var->getBlockPath();
		std::string name = bpath;
		name.append(".").append(var->getName());
		RFSMvalueEval val = this->getLocalValue(name);
	}
	else{
		bpath = blockpath;
	}
	
	std::string dbgmsg;
	std::vector<std::string> path = Estring::split(bpath, ".");

	for(unsigned int k=path.size(); k>0; k--){
		std::string c_path="";
		for(unsigned int i=0; i<k; i++){
			c_path.append(path.at(i));
			if(i+1<k)c_path.append(".");
		}

		ValueMap::iterator it;
		for(it = m_valueTAB.begin(); it!=m_valueTAB.end();it++){
			std::string	vname = it->first;
			size_t found;
			found=vname.find_last_of(c_path);
			if (found!=std::string::npos){
				RFSMvalueEval value = it->second;
				if(value.getLine()<=line){
					dbgmsg = makeDebugMsg(this->getFileName(), bpath, vname.substr(found+2), value);//+2�� 0.k���� ��(.)�ǳʱ�
					sendMonitorMsg(dbgmsg);
				}
			}
		}
	}
}
*/

std::string ExeTaskTreeNode::makeDebugMsg(std::string taskname, std::string file, int dbgLine, std::string name, RFSMvalueEval val)
{
	MonitorResponse* monRes = getMonitorRes();
	monRes->init(RES_DATA_lvar);
	monRes->addVariable(name, val);

	std::string monMsg = monRes->makeResponseMsg(taskname, getCreatedThreadID(), getCallStmtLine(), file, dbgLine, val.getTarget(), val.getLine());
	//printf("%s\n", monMsg.c_str());

	return monMsg;
}

std::string ExeTaskTreeNode::makeEmptyDebugMsg(std::string taskname, std::string file, int dbgLine)
{
	MonitorResponse* monRes = getMonitorRes();
	monRes->init(RES_DATA_empty);

	std::string monMsg = monRes->makeResponseMsg(taskname, getCreatedThreadID(), getCallStmtLine(), file, dbgLine, "", -1);
	//printf("%s\n", monMsg.c_str());

	return monMsg;
}

void ExeTaskTreeNode::sendMonitorMsg(std::string msg)
{
	m_svr->writeMsg2Sock(msg);
/*
pthread_mutex_lock(&m_lmu);

	//������ �ٿ��� ����
	std::string head = "";
	std::string sizeStr = int2strFP(msg.size(), 8);
	head.append(sizeStr).append(msg);

#ifdef _WIN32
	send(m_client, (char*)head.c_str(), head.size(), 0);	
#else
	write(m_client, (char*)head.c_str(), head.size());
#endif

pthread_mutex_unlock(&m_lmu);
*/
}

RemoteCLI* ExeTaskTreeNode::getRemoteCLI()
{
	return m_svr;
}

MonitorResponse* ExeTaskTreeNode::getMonitorRes()
{
	return m_monRes;
}

void ExeTaskTreeNode::sendThreadInfo(std::string cmd)
{
	//pthread_t thid = getThread_t();
	//int a = (int)thid;

	int a = getThreadNum();
	int b = getParentThreadID();
	
	std::string threadMsg = "RES-start;thread;";	
	threadMsg.append("@thid").append("[").append(int2str(a)).append("]");
	threadMsg.append("@parent").append("[").append(int2str(b)).append("]");
	threadMsg.append("@name").append("[").append(m_nodeName).append("]");
	threadMsg.append("@cmd").append("[").append(cmd).append("]");
	threadMsg.append("@threadLine").append("[").append(int2str(getCallStmtLine())).append("]");
	threadMsg.append("@file").append("[").append(getFileName()).append("]");
	threadMsg.append(";RES-end");
	sendMonitorMsg(threadMsg);
}

ExeTaskTreeNode* ExeTaskTreeNode::getParent()
{
	return m_parent;
}

/**
	parent thread id�� �ٷ� ���� parent�� thread id�� �ƴ� ���� �ִ�.
	���� parent�� thread�� ������� ���� ���� �ֱ� �����̴�.
*/
int ExeTaskTreeNode::getParentThreadID()
{
	if(m_parent == NULL){
		//�ֻ����� ��� ExeTask���
		return 0;
	}
	else{
		int id = m_parent->getThreadNum();
		if(id==0){
			//���� parent�� �����尡 �ƴѰ��
			return m_parent->getParentThreadID();
		}
		else{
			return id;
		}
	}
}

int ExeTaskTreeNode::getCreatedThreadID()
{
	int id = (int)getThreadNum();
	if(id==0){
		//���� parent�� �����尡 �ƴѰ��
		return m_parent->getCreatedThreadID();
	}
	else{
		return id;
	}
}

ValueMap* ExeTaskTreeNode::getVariableTAB()
{
	return &m_valueTAB;
}

void ExeTaskTreeNode::setConstructed()
{
	m_isConstructed = true;
}

void ExeTaskTreeNode::setDestructed()
{
	m_isDestructed = true;
}

bool ExeTaskTreeNode::isConstructed()
{
	return m_isConstructed;
}

bool ExeTaskTreeNode::isDestructed()
{
	return m_isDestructed;
}

void ExeTaskTreeNode::setAsMission()
{
	m_isMission = true;
}

bool ExeTaskTreeNode::isMission()
{
	return m_isMission;
}

bool ExeTaskTreeNode::isJoining()
{
	return m_joining;
}

void ExeTaskTreeNode::setNextTarget(std::string nextTarget)
{
	m_nextTarget = nextTarget;
}

std::string ExeTaskTreeNode::getNextTarget()
{
	return m_nextTarget;
}