/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "run/control/TaskRunner.h"

TaskRunner::TaskRunner(ExeTask* eTask, ModelBinder* binder, LogFile* logFile)
{
	m_logFile	= logFile;
	m_binder	= binder;
	m_eTask		= eTask;
	m_traceThreadON = TaskConfig::getIsThreadTrace();

	m_exitActionPerformed = false;

	m_exeMode		= NORMAL;
	m_suspend		= false;

	pthread_cond_init(&m_suspend_cond, NULL);
	pthread_mutex_init(&m_suspend_mu, NULL);
}

TaskRunner:: ~TaskRunner(void)
{
	//m_logFile�� TaskRunner�� ����ϴ� �ܺ� ��, bhv���� ����ϴ� �Ŷ� ���� �� ����ϱ� ������
	//���⼭ �������� �ʴ´�.
	pthread_cond_destroy(&m_suspend_cond);
	pthread_mutex_destroy(&m_suspend_mu);
}

bool TaskRunner::enterConstruct(ExeTaskTreeNode* node)
{
	RFSMcst* cst = NULL;
	if(typeid(*node)==typeid(ExeTask)){
		cst = m_eTask->getTask();
	}
	else if(typeid(*node)==typeid(ExeBehavior)){
		cst = m_eTask->getBehavior(node->getNodeName());
	}
	else if(typeid(*node)==typeid(ExeConnector)){
		cst = m_eTask->getConnector(node->getNodeName());
	}

	if(cst != NULL){
		std::vector<RFSMstmt*>* cons = cst->getConstruct();
		//construct ����
		if(cons != NULL){
			FLOG_TRACE(m_logFile, " [CONSTRUCT] ");		
			m_eTask->setCurTarget(node->getNodeName() + ".construct"); 	
			enterActionBlock(cons, node);
			return true;
		}		
	}
	return false;
}

void TaskRunner::enterDestruct(ExeTaskTreeNode* node)
{
	if(node->isDestructed()) return;
	node->setDestructed();

	//�ڽ��� �ִٸ�
	std::vector<ExeTaskTreeNode*> clist = node->getAllChildNodes();
	if(clist.size()>0){
		for(unsigned int j=0; j<clist.size(); j++){
			enterDestruct(clist[j]);
		}
	}
	
	RFSMtreenode* cst = NULL;
	if(typeid(*node)==typeid(ExeTask)){
		cst = m_eTask->getTask();
	}
	else if(typeid(*node)==typeid(ExeBehavior)){
		cst = m_eTask->getBehavior(node->getNodeName());
	}
	else if(typeid(*node)==typeid(ExeConnector)){
		cst = m_eTask->getConnector(node->getNodeName());
	}

	if(cst != NULL){	
		std::vector<RFSMstmt*>* dest = cst->getDestruct();
		if(dest != NULL){
			FLOG_TRACE(m_logFile, " [DESTRUCT] ");		
			m_eTask->setCurTarget(node->getNodeName() + ".destruct"); 
			enterActionBlock(dest, node);
		}	

		onBreak(node, cst, cst->getBlockID());
	}	
	//delete node;
}

/*
void TaskRunner::enterTaskLoop(ExeTask* exeTask, RFSMtask* rfsmtask)
{
	FLOG_TRACE(m_logFile, "[Task: %s] ", exeTask->getNodeName().c_str());

	//1.<Construct ����>
	if(!exeTask->isConstructed()){
		exeTask->setConstructed();
		//1.1 Var
		std::vector<RFSMstmt*>* varAsmts = rfsmtask->getVarAsmts();
		if(varAsmts != NULL){//varAsmts������ RFSMasmt�� RFSMvar �ΰ��� ����			
			FLOG_TRACE(m_logFile, " [VAR]");	
			exeTask->setCurTarget(exeTask->getNodeName() + ".var");
			enterActionBlock(varAsmts, exeTask);
		}
		//1.2 construct ��� ����
		enterConstruct(exeTask);
		//return;
	}

	//2.<expand�� behavior|conexer ����>
	std::vector<ExeTransition*> translist;
	std::vector<RFSMstmt*>* runStmts = rfsmtask->getRunBlock();
	RFSMStmtBlock* enclosing = NULL;
	if(runStmts != NULL && runStmts->size() > 0){
		FLOG_TRACE(m_logFile, " [RUN]");
		exeTask->setCurTarget(exeTask->getNodeName() + ".run"); 
		enterDecisionBlock(runStmts, exeTask, translist, false);
	}

	//3.<expand ����>
	//3.1 ���� ���¿� ���� ���� ��, target�� nothing�� ���� not valid�� ������ �Ѵ�.
	removeNotValidSubs(exeTask, translist);	
	
	//3.2 �� ��� �����Ͽ� ����
	link4translist(exeTask, translist);

	//3.3 ���õ� ���� ����� �ϷῩ�� �˻�
    if(translist.size() > 0){		
		std::string target = translist[0]->getName();
		int line = translist[0]->getStmtLine();
		ExeTaskTreeNode* childnode = exeTask->getChildNode(target, line);
		if(childnode != NULL && childnode->isCompleted()){
			exeTask->setCompleted(true);
			enterDestruct(childnode);
			exeTask->removeChildNode(target, true, line);
		}
	}
	//3.4 translist ����
	std::vector<ExeTransition*>::iterator it;
	for(it=translist.begin() ; it!=translist.end() ; ++it){
		delete (*it);
	}
	translist.clear();
}
*/

void TaskRunner::updateConnectorStatus(ExeTaskTreeNode* cur, ExeTaskTreeNode* sub)
{
	if(typeid(*sub)==typeid(ExeConnector)){
		ExeConnector* subExeCon = (ExeConnector*)sub;
		std::vector<ExeTaskTreeNode*>* cNodes = subExeCon->getAllChildNodesP();

		if(cNodes==NULL) return;

		//�ϴ� sub��带 ���¸� �˻�
		for(unsigned int i=0; i<cNodes->size();i++){
			updateConnectorStatus(subExeCon, cNodes->at(i));
		}

		int conType = subExeCon->getConType();
		int joinType = subExeCon->getJoinType();				
		bool iscompleted = false;
		if(subExeCon->isRunnedAfterConstBL() && cNodes->size()<1){
			//connector�� ���� ��尡 ������
			subExeCon->setCompleted(true);
		}		
		else if(conType == CONEXER && joinType==AND_CONNECTOR){
			iscompleted = true;
			for(unsigned int i=0; i<cNodes->size();i++){
				if(cNodes->at(i) != NULL && !cNodes->at(i)->isCompleted()){
					iscompleted = false;
					break;
				}
			}
			if(iscompleted){
				//�Ϸ�Ǹ�
				enterChildNodeDestruct(subExeCon);
				subExeCon->removeAllChildNodes(false); //isTaskEnd=false
				subExeCon->setCompleted(true);
printf("%s: an and-join is completed\n", subExeCon->getNodeName().c_str());
			}
		}
		else if(conType == CONEXER && joinType==OR_CONNECTOR){
			for(unsigned int i=0; i<cNodes->size();i++){
				if(cNodes->at(i) != NULL && cNodes->at(i)->isCompleted()){
					iscompleted = true;					
					subExeCon->stopRunning();//���� ����						
					enterChildNodeDestruct(subExeCon);
					subExeCon->removeAllChildNodes(false); //isTaskEnd=false
					break;
				}
			}
			if(iscompleted){
				//�Ϸ�Ǹ�
				subExeCon->setCompleted(true);
printf("%s: an or-join is completed\n", subExeCon->getNodeName().c_str());
			}
		}
		else if(conType == SEQEXER && !cNodes->empty()){
			unsigned int seq = subExeCon->getCurActiveSeq(); 
			std::string sub	= cNodes->at(seq-1)->getNodeName();
			int callstmtline = cNodes->at(seq-1)->getCallStmtLine();
			ExeTaskTreeNode* childnode = subExeCon->getChildNode(sub, callstmtline);
			if(childnode != NULL && childnode->isCompleted()){
				//��ü �Ϸ�
				if(subExeCon->getCurActiveSeq() == subExeCon->getNumOfRunBlocks()){
					subExeCon->setCompleted(true);
printf("%s: an seqexer is completed\n", subExeCon->getNodeName().c_str());
				}
				//�߰���� �Ϸ�
				else{
					subExeCon->setCurActiveSeq(seq+1);
				}
			}
		}

		if(iscompleted){
			RFSMconnector* rfsmCon		= m_eTask->getConnector(subExeCon->getNodeName());	
			RFSMStmtBlock* withsblock	= rfsmCon->getRunBlocks();
			onBreak(cur, withsblock, withsblock->getBlockID());
		}
	}
}

void TaskRunner::monitorSubOfRun(ExeTaskTreeNode* cur, ExeTaskTreeNode* sub)
{
	FLOG_TRACE(m_logFile, " [RUN]");
	m_eTask->setCurTarget(m_eTask->getNodeName() + ".run"); 

	//std::vector<ExeTaskTreeNode*>* subs = m_eTask->getAllChildNodesP();
	//ExeTaskTreeNode* node = subs->at(0);
	if(typeid(*sub)==typeid(ExeBehavior)){
		ExeBehavior* exeBhvSub = (ExeBehavior*)sub;
		std::string curSName = exeBhvSub->getCurState();
		RFSMbehavior* bhv = m_eTask->getBehavior(exeBhvSub->getNodeName());
		RFSMstate* state = bhv->getState(curSName);
		if(state != NULL) enterState(exeBhvSub, bhv, state);
		if(exeBhvSub != NULL && exeBhvSub->isOnGoalState() && exeBhvSub->isEntryExecuted()){
		//if(exeBhv != NULL && exeBhv->isCompleted()){
			exeBhvSub->setCompleted(true);
			enterDestruct(exeBhvSub);
			m_eTask->setCompleted(true);
			m_eTask->removeAllChildNodes(true);
		}
	}
	else if(typeid(*sub)==typeid(ExeConnector)){
		updateConnectorStatus(cur, sub);
	}
}

void TaskRunner::enterTask(ExeTask* exeTask, RFSMtask* rfsmtask)
{
	FLOG_TRACE(m_logFile, "[Task: %s] ", exeTask->getNodeName().c_str());

	//1.<Construct ����>
	if(!exeTask->isConstructed()){
		exeTask->setConstructed();
		//1.1 Var
		std::vector<RFSMstmt*>* varAsmts = rfsmtask->getVarAsmts();
		if(varAsmts != NULL){//varAsmts������ RFSMasmt�� RFSMvar �ΰ��� ����			
			FLOG_TRACE(m_logFile, " [VAR]");	
			exeTask->setCurTarget(exeTask->getNodeName() + ".var");
			enterActionBlock(varAsmts, exeTask);
		}
		//1.2 construct ��� ����
		enterConstruct(exeTask);
		//return;

		//2.<expand�� behavior|conexer ����>
		std::vector<ExeTransition*> translist;
		std::vector<RFSMstmt*>* runStmts = rfsmtask->getRunBlock();
		RFSMStmtBlock* enclosing = NULL;
		if(runStmts != NULL && runStmts->size() > 0){
			FLOG_TRACE(m_logFile, " [RUN]");
			exeTask->setCurTarget(exeTask->getNodeName() + ".run"); 
			enterDecisionBlock(runStmts, exeTask, translist, false);
		}

		//3.<expand ����>
		//3.1 ���� ���¿� ���� ���� ��, target�� nothing�� ���� not valid�� ������ �Ѵ�.
		removeNotValidSubs(exeTask, translist);	
		
		//3.2 �� ��� �����Ͽ� ����
		link_Expand(exeTask, translist);

		//3.3 ���õ� ���� ����� �ϷῩ�� �˻�
		/* //�ѹ� ���õ� sub��带 �ٲܶ� (��������)
		if(translist.size() > 0){		
			std::string target = translist[0]->getName();
			int line = translist[0]->getStmtLine();
			ExeTaskTreeNode* childnode = exeTask->getChildNode(target, line);
			if(childnode != NULL && childnode->isCompleted()){
				enterDestruct(childnode);
				exeTask->setCompleted(true);				
				exeTask->removeChildNode(target, true, line);
			}
		}*/
		//3.4 translist ����
		std::vector<ExeTransition*>::iterator it;
		for(it=translist.begin() ; it!=translist.end() ; ++it){
			delete (*it);
		}
		translist.clear();

	}
	//������尡 ������ ����
	else{
		std::vector<ExeTaskTreeNode*>* subs = exeTask->getAllChildNodesP();
		if(subs->empty()){
			exeTask->setCompleted(true);
		}
		else{
			ExeTaskTreeNode* subnode = subs->at(0);
			monitorSubOfRun(exeTask, subnode);
			if(subnode->isCompleted()) exeTask->setCompleted(true); 
		}
	}
}

void TaskRunner::enterConnector(ExeConnector* exeCon, ValueMap params)
{
	FLOG_TRACE(m_logFile, "[CONNECTOR: %s] ", exeCon->getNodeName().c_str());	

	//1. var ����
	if(params.size()>0){
		m_eTask->setCurTarget(exeCon->getNodeName() + ".param"); 
		const std::string taskBlockID = exeCon->getBlockID();
		ValueMap::iterator it;
		for(it=params.begin(); it!=params.end() ;it++){
			//ȣ���� ��ġ�� ��(RFSMValueEval)�� �״�� ���� �Ǿ��� ������, �Ķ������ ��ġ�� target���� �ٲ�� �Ѵ�. 
			it->second.setLine(exeCon->getNodeNameLine()); 
			it->second.setTarget(m_eTask->getCurTarget());
			//�̸��� �½�ũ ���� �ٿ��� �Ѵ�.
			std::string localVarName = taskBlockID;
			localVarName.append(".").append(it->first);			
			exeCon->addLocalValue(localVarName, it->second); //behavior �Ķ���ʹ� �׳� ������ 0���� ����
		}
		params.clear();
		//delete params;//���߿� auto_ptr�� �ٲ���...
	}

	//2. construct ����
	RFSMconnector* rfsmCon = m_eTask->getConnector(exeCon->getNodeName());	
	std::vector<RFSMstmt*>* varAsmts = rfsmCon->getVarAsmts();
	if(!exeCon->isConstructed()){
		exeCon->nodeStart();//BEHAVIOR_TIME����
		exeCon->setConstructed();
		//2.1 Behvaior Var
		if(varAsmts != NULL){			
			FLOG_TRACE(m_logFile, " [VAR]");	
			m_eTask->setCurTarget(exeCon->getNodeName() + ".var");
			enterActionBlock(varAsmts, exeCon);
		}
		//2.2 Construct
		enterConstruct(exeCon);
		//return;
	}

	//3. ���ǽ��� ���� ���(with�� ����)
	exeCon->enterAfterConstBL(true);
	std::vector<ExeTransition*> translist;
	RFSMStmtBlock* withsblock = rfsmCon->getRunBlocks();
	if(withsblock != NULL){
		std::vector<RFSMstmt*>*	withstmts = withsblock->getStmtBlock();
		enterDecisionBlock(withstmts, exeCon, translist, true);
	}
	exeCon->setWithList(translist);

	//4 translist�� ���Ե� ������ transitionó��
	link_Expand(exeCon, translist);
}

void TaskRunner::enterBehavior(ExeBehavior* exeBhv, ValueMap params)
{
	//_CrtDumpMemoryLeaks();
//#ifdef	__DEBUG_WORKER_RUNNING_
//	std::string msg = "(RUNNING:TASK) ";
//	msg.append(exeTask->getName());
//	printMsg(msg);
//#endif
	FLOG_TRACE(m_logFile, "[BEHAVIOR: %s] ", exeBhv->getNodeName().c_str());	

	if(params.size()>0){
		m_eTask->setCurTarget(exeBhv->getNodeName() + ".param"); 
		const std::string taskBlockID = exeBhv->getBlockID();
		ValueMap::iterator it;
		for(it=params.begin() ; it!=params.end() ; it++){
			//ȣ���� ��ġ�� ��(RFSMValueEval)�� �״�� ���� �Ǿ��� ������, �Ķ������ ��ġ�� target���� �ٲ�� �Ѵ�. 
			it->second.setLine(exeBhv->getNodeNameLine()); 
			it->second.setTarget(m_eTask->getCurTarget());
			//�̸��� �½�ũ ���� �ٿ��� �Ѵ�.
			std::string localVarName = taskBlockID;
			localVarName.append(".").append(it->first);			
			exeBhv->addLocalValue(localVarName, it->second); //behavior �Ķ���ʹ� �׳� ������ 0���� ����
		}
		params.clear();
		//delete params;//���߿� auto_ptr�� �ٲ���...
	}
	
	//1. Behavior �����ν��Ͻ��� �̸����� ���� RFSMbehavior(����)�� ���´�.
	RFSMbehavior* bhv = m_eTask->getBehavior(exeBhv->getNodeName());	

	//2. construct ����
	if(!exeBhv->isConstructed()){
		std::vector<RFSMstmt*>* varAsmts = bhv->getVarAsmts();
		exeBhv->nodeStart();//BEHAVIOR_TIME����
		exeBhv->setConstructed();
		//2.1 Behvaior Var
		if(varAsmts != NULL){			
			FLOG_TRACE(m_logFile, " [VAR]");	
			m_eTask->setCurTarget(exeBhv->getNodeName() + ".var");
			enterActionBlock(varAsmts, exeBhv);
		}
		//2.2 Construct ���� �����ϸ� EOI(return)
		//if(enterConstruct(exeBhv))return;
		enterConstruct(exeBhv);
	}

	//3. Behavior�� �����ν��Ͻ��� ���� ���� ���¸� �ľ�
	std::string curSName = exeBhv->getCurState(); //Task�� ���� State�� ������

	//4. ���� ������ ������ ��´�.
	RFSMstate* state = bhv->getState(curSName);

	//5. ���¸� ����
	if(state != NULL) enterState(exeBhv, bhv, state);

}

/*
[����� ���� ����]
����
  Task���� ���ʱ�state ��(Entry) �� (Do)���ѹ��� ����Ŭ ��
���Է�
  ���1:(Transition:stay)��(Do)�� �ѹ��� ����Ŭ ��
  ���2:(Transition:stay)��(Do)��(Exit)�� Subtask���� ��(Entry) ��(Do) ���ѹ��� ����Ŭ ��
  ���3:(Transition:state)��(Exit)��ٸ�State��(Entry) ��(Do) ���ѹ��� ����Ŭ ��

*/
void TaskRunner::enterState(ExeBehavior* ebhv, RFSMbehavior* bhv, RFSMstate* state)
{
//#ifdef	__DEBUG_WORKER_RUNNING_
//	std::string msg = "(RUNNING:STATE) ";
//	msg.append(state->getName());
//	printMsg(msg);
//#endif
	//std::string bhvName = ebhv->getNodeName();

	std::string stName	= state->getName();

	if(m_exitActionPerformed){
		m_exitActionPerformed = false;
		return;
	}

	bool actionPerformed = false;

	FLOG_TRACE(m_logFile, " [STATE: %s] ", stName.c_str());	

	//1.Entry & Do	
	std::vector<RFSMstmt*>* varAsmts = state->getVarAsmts();
	std::vector<RFSMstmt*>* entrystmts = state->getConstruct();
	if(!ebhv->isEntryExecuted()){
		ebhv->entryExecuted();
		if(varAsmts != NULL){
			FLOG_TRACE(m_logFile, " (VAR)--------------");
			enterActionBlock(varAsmts, ebhv);
		}
		//Entry ����
		if(entrystmts != NULL){
			m_eTask->setCurTarget(stName + ".entry"); //stname�� behavior�̸��� ����
			//updateMonitoringStatus();
			FLOG_TRACE(m_logFile, " (ENTRY)--------------");
			ebhv->setCurStateBlock(ENTRY_BLOCK);
			//actionPerformed = enterActionBlock(entrystmts, ebhv);
			//if(actionPerformed) return;
			//entry�� ��� �־ �׳� iteration�� �����°� ����
			enterActionBlock(entrystmts, ebhv);
		}
		return;
	}

	//2.Decision:���� õ�̵��� ����
	std::vector<ExeTransition*> translist;
	std::string nextTarget = ebhv->getNextTarget();
	if(nextTarget != ""){
		//2.1 ���� ���� ���� behavior�� ���� behavior�� �Ϸ�ǰ� ~>�� ���� õ�̵Ǵ� ���
		ExeTransition* t = new ExeTransition(TO_STATE, nextTarget, "", -1, -1);
		ebhv->setNextTarget("");
		translist.push_back(t);
	}
	else{
		std::vector<RFSMstmt*>* trans = state->getTransition();
		if(trans!= NULL && trans->size()>0){
			m_eTask->setCurTarget(stName + ".trans");//stname�� behavior�̸��� ����
			//updateMonitoringStatus();
			FLOG_TRACE(m_logFile, " (TRANSITION)---------");
			ebhv->setCurStateBlock(TRANS_BLOCK);
			//moveto = enterDecisionBlock(trans, ebhv);
			enterDecisionBlock(trans, ebhv, translist, false);
		}
	}

	//transition block������ ������ϴٰ� ���� �����ϸ�
	if(m_eTask->isStopped()){
		for(unsigned int i=0; i<translist.size();i++) delete translist[i];
		translist.clear();	
		return;
	}
	
	int transNum = translist.size();
	bool isState = false;
	bool isBhv = false;	
	bool isCon = false;
	ExeTransition* moveto = NULL;
	if(transNum > 0){		
		for(int i=0; i<transNum;i++){
			ExeTransition* trans = translist[i];
			if(trans->getTransType()==TO_BEHAVIOR)
				isBhv = true;
			if(trans->getTransType()==TO_CONNECTOR)
				isCon = true;
			else if(trans->getTransType()==TO_STATE)
				isState = true;
		}

		if(transNum >= 2 && isState){
//���� ��� bebavior�� state�� ���ÿ� ���� �� �� ����.
//�ټ��� behavior�� ���ķ� ���� ���̰� �����ϴ�.
		}
		
		if(transNum == 1 && isState){
			moveto = translist[0];
		}
	}

	//���� �ִ°��, transition ���ǿ��� �����Ǵ� �� �ƹ��͵� ����.
	if(translist.size() < 1){
		//�ƹ��͵� ���Ѵ�.
		//delete moveto;

		return;
	}
	//õ�̵Ǵ� ���
	else{
		//3. decision ��� ȹ��			
		if(transNum == 1 && isState){ //3.1 moveto�� ���
			FLOG_TRACE(m_logFile, "   (RUNNING:Goto) %s ", moveto->getName().c_str());
			std::string target = moveto->getName();

//this�ΰ�� isBehavior�� false���� Ȯ�ο��
			
			//4.Do	
			if(target.compare("stay")==0){
				//���� ���¿� �ӹ�����.
				std::vector<RFSMstmt*>* dostmts = state->getDostmts();
				if(dostmts != NULL){
					m_eTask->setCurTarget(stName + ".stay"); //stname�� behavior�̸��� ����
					//updateMonitoringStatus();
					FLOG_TRACE(m_logFile, " (STAY)-----------------");
					ebhv->setCurStateBlock(STAY_BLOCK);
					//actionPerformed = enterActionBlock(dostmts, ebhv);
					//if(actionPerformed){
					//	delete moveto;
					//	return;
					//}
					enterActionBlock(dostmts, ebhv);
					delete moveto;
					return;
				}
			}
	
			else{								
				/************************
					���� �߻�
				************************/			
				//5.Exit
				if(transNum == 1 && isState){										
					std::vector<RFSMstmt*>* exitstmts = state->getDestruct();
					if(exitstmts != NULL){
						//exit���� stmt�� �����ϴ� ���
						m_eTask->setCurTarget(stName + ".exit"); //stname�� behavior�̸��� ����
						//updateMonitoringStatus();
						FLOG_TRACE(m_logFile, " (EXIT)---------------");		
						ebhv->setCurStateBlock(EXIT_BLOCK);
						//actionPerformed = enterActionBlock(exitstmts, ebhv);
						//if(actionPerformed){
						//	m_exitActionPerformed = true;
						//}
						enterActionBlock(exitstmts, ebhv);
						m_exitActionPerformed = true;
					}	
					//exit�� �ִ� ���� exit�� �����ߴٰ� ����
					//exit�� �����ϰ� ������ ���̰� �߻��Ѵ�.���� �������� actionPerfromed�� ����
					ebhv->exitExecuted();
		
					//ebhv->setBehaviorInCurState(NULL); //���� ������ subbehavior���� delete�ϱ� ����
					RFSMstate* newState = bhv->getState(target);										
					enterChildNodeDestruct(ebhv);
					ebhv->removeAllChildNodes(false); //isTaskEnd = false
					ebhv->setCurState(target);
					
					std::string blockpath = bhv->getBlockID().append(".").append(state->getBlockID());
					onBreak(ebhv, state, blockpath);

					enterState(ebhv, bhv, newState);				
				}		
			}
			delete moveto;
		}
		else if(isBhv || isCon){//3.2 expand�� ���
			removeNotValidSubs(ebhv, translist);		
			link_Expand(ebhv, translist);
			std::vector<ExeTransition*>::iterator it;
			for(it=translist.begin() ; it!=translist.end() ; ++it) delete (*it);
			translist.clear();	
		}		
	}
}

void TaskRunner::enterDecisionBlock(std::vector<RFSMstmt*>* stmts, ExeTaskTreeNode* etask, std::vector<ExeTransition*>& translist, bool isPar)
{
	ExeTransition* transition = NULL;

	if(stmts != NULL && stmts->size()>0){
		for(unsigned int i=0 ; i<stmts->size(); i++){
			RFSMstmt* stmt = (*stmts)[i];
			bool isOnBreakStmt = false;

			if(m_exeMode == MONITOR && m_suspend){
				pthread_mutex_lock(&m_suspend_mu);
printf("Monitor-Suspend(line:%d, thread:%d)\n", stmt->getEndLine(), etask->getThreadNum());
				pthread_cond_wait(&m_suspend_cond, &m_suspend_mu);
printf("Monitor-Resume(line:%d, thread:%d)\n", stmt->getEndLine(), etask->getThreadNum());
				pthread_mutex_unlock(&m_suspend_mu);
			}

			//RFSMblockStmt�� ���� �˻�� �ݵ�� isBreakline���� ���� �;� �Ѵ�. 
			//����� �� '}'�� ��ġ�� RFSMblockStmt�� EOL�̱� ������ ��ϳ��� stmt�� �� ����ǰ� ��� ���� �극��ũ �Ǿ�� �Ѵ�.
			// transition�� run�� RFSMblockStmt�� root�̴�.
			if(stmt->getClassName()==CLS_RFSMblockStmt){
				RFSMStmtBlock* bstmt = (RFSMStmtBlock*)stmt;
				enterDecisionBlock(bstmt->getStmtBlock(), etask, translist, false);
			}

			else //�̺κ��� Ȱ��ȭ �Ǹ� ������ ��ȣ�� �����ϰ� �ȴ�. 
				isOnBreakStmt = onBreak(etask, stmt, stmt->getBlockID());
			//if(isBreakline(etask, stmt->getFileName(), stmt->getEOL())) {
				//1. ���� ���̺� �˻��� ���� ���� ����. ���� ���̺� blockpath�� ���̵�..�̿�
			//	etask->sendDebugMsg(m_eTask->getTask()->getName(), stmt->getEOL(), stmt->getBlockPath());
				//2. ��� 
			//	if(m_exeMode == DEBUG) waitAtBreakline(etask, stmt);
			//}

			//if(typeid(*stmt)==typeid(RFSMifStmt) ){
			if(stmt->getClassName().compare(CLS_RFSMifStmt)==0){
				RFSMifStmt* ifstmt = (RFSMifStmt*)stmt;
				//transition = enterDecisionStmt(ifstmt, etask);
				//���� �� -> true, false
				RFSMboolable* cond = ifstmt->getCondition();
				bool result = evalCondition(cond, etask, ifstmt->getEndLine());

				//�򰡿� �ش�Ǵ� statements
				std::vector<RFSMstmt*>* bStmts = NULL;
				if(result) bStmts = ifstmt->getTrueStmts();
				else bStmts = ifstmt->getFalseStmts();

				unsigned int beforeSize = translist.size();
				enterDecisionBlock(bStmts, etask, translist, isPar);
				if(translist.size()>beforeSize){
					//�ſ� �߿��� ���̰� �߻��ϸ� (���������� ���� if���� ������) �ٸ� if���� ���̻� ���� ���ƾ� �Ѵ�.
					break;
				}
			}
			else if(stmt->getClassName().compare(CLS_RFSMvar)==0){
				//���� �׳� �������̶� ������ ����
			}
			//else if(typeid(*stmt)==typeid(RFSMasmt) ){
			else if(stmt->getClassName().compare(CLS_RFSMasmt)==0){
				//evaluateValue���� ���ú����� ����ȴ�.
				RFSMasmt* asmt = (RFSMasmt*)stmt;
				int el = ((RFSMstmt*)asmt)->getEndLine(); //RFSMstmt*�� RFSMvalue* �ΰ��߿� �ϳ��� casting
				evalValue(asmt, etask, el);//����ó���� evaluateValue������...
			}
			//else if(typeid(*stmt)==typeid(RFSMcallFunc) ){
			//�׳� �Լ� ȣ���̴�. ���� assignment������ �������� �׳� �Լ������� ��쵵 �ִ�.
			else if(stmt->getClassName().compare(CLS_RFSMcallFunc)==0){

				ValueMap* paramsEval = NULL;
				RFSMcallFunc* rfunc = (RFSMcallFunc*)stmt;
				std::vector<RFSMparam*>* params = rfunc->getParameters();
				if(params != NULL){
					int el = ((RFSMvalue*)rfunc)->getEndLine(); //RFSMstmt*�� RFSMvalue* �ΰ��߿� �ϳ��� casting
					paramsEval = evalParams(params, etask, el);
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////
				//
				if(params != NULL && paramsEval == NULL){
					FLOG_ERROR(m_logFile, "   (RUNNING:FUNCTION) %s, Calling function is skipped.", rfunc->toString().c_str());
				}
				else{					
					//m_binder->callFunction(m_logFile, RFSMcall::CALL_FUNC, rfunc->getType(), rfunc->getName(), paramsEval);
					bind4Function(etask, CALL_FUNC, rfunc, paramsEval);
//printf("[EOL:RFSMcallFunc]%s(%d): %d\n", rfunc->getBlockFullName().c_str(), etask->getThreadID(), rfunc->getEOL());
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////

				if(paramsEval != NULL) delete paramsEval;
			}
			//subBehavior�� ȣ���ϴ� ���
			else if(stmt->getClassName().compare(CLS_RFSMcallBehavior)==0){

				ValueMap* paramsEval = NULL;
				RFSMcallBhv* bhv = (RFSMcallBhv*)stmt;
				std::vector<RFSMparam*>* params = bhv->getParameters();
				if(params != NULL){
					paramsEval = evalParams(params, etask, bhv->getEndLine());
				}

				if(params != NULL && paramsEval == NULL){
					FLOG_ERROR(m_logFile, "   (RUNNING:TASK) %s, Calling Sub-bhv is skipped.", bhv->toString().c_str());
				}
				else{
					ExeTransition* t = new ExeTransition(TO_BEHAVIOR, bhv->getName(), bhv->getNextTarget(), stmt->getEndLine(), bhv->getPeriod());
					t->setParameters(paramsEval);
					translist.push_back(t);
					if(!isPar) return; 
				}				
			}
			else if(stmt->getClassName().compare(CLS_RFSMcallConnector)==0){
				
				ValueMap* paramsEval = NULL;
				RFSMcallConnector* con = (RFSMcallConnector*)stmt;
				std::vector<RFSMparam*>* params = con->getParameters();
				if(params != NULL){
					paramsEval = evalParams(params, etask, con->getEndLine());
				}

				if(params != NULL && paramsEval == NULL){
					FLOG_ERROR(m_logFile, "   (RUNNING:TASK) %s, Calling a connector is skipped.", con->toString().c_str());
				}
				else{
					ExeTransition* t = new ExeTransition(TO_CONNECTOR, con->getName(), con->getNextTargetState(), stmt->getEndLine(), con->getPeriod());
					t->setParameters(paramsEval);
					translist.push_back(t);
					if(!isPar) return; 

				}				
			}
			//�ٸ� state�� ���̵Ǵ� ���
			else if(stmt->getClassName().compare(CLS_RFSMgotoStmt)==0){
				RFSMgotoStmt* gotostmt = (RFSMgotoStmt*)stmt;
				ExeTransition* t = new ExeTransition(TO_STATE, gotostmt->getName(), "", stmt->getEndLine(), -1);
				translist.push_back(t);
				if(!isPar) return;				
			}
			else if(stmt->getClassName().compare(CLS_RFSMsymbStmt)==0){
				//�׳� ������ a;
			}

			else if(stmt->getClassName()==CLS_RFSMparStmt){
				FLOG_TRACE(m_logFile, " PARALLEL:");	
				RFSMStmtBlock* bstmt = (RFSMStmtBlock*)stmt;
				std::vector<RFSMstmt*>* withstmts = bstmt->getStmtBlock();
				if(withstmts != NULL && withstmts->size() >0){
					enterDecisionBlock(withstmts, etask, translist, true);
				}		
			}
			else if(stmt->getClassName()==CLS_RFSMwithStmt){
				//FLOG_TRACE(m_logFile, "  +THEN");	
				RFSMStmtBlock* bstmt = (RFSMStmtBlock*)stmt;
				std::vector<RFSMstmt*>* stmts = bstmt->getStmtBlock();
				if(stmts != NULL && stmts->size() >0){
					enterDecisionBlock(stmts, etask, translist, false);
				}				
			}
			else{
				//�������� error
			}
			//stay�� ���� ���ؼ� .....
		}
	}	
}

void TaskRunner::removeNotValidSubs(ExeTaskTreeNode* curNode, std::vector<ExeTransition*> translist)
{
	//1. ���ο� ���̿� ���ԾȵǴ� sub node�� ����
	std::vector<ExeTaskTreeNode*> blist = curNode->getAllChildNodes();
	if(blist.size()>0){
		for(unsigned int j=0; j<blist.size(); j++){//���� ���Ե� sub behavior
			//ExeBehavior* pBhv = (ExeBehavior*)blist[j];
			std::string nodeName = blist[j]->getNodeName();	
			//std::string nodeId	 = blist[j]->getBlockID();
			int callLine = blist[j]->getCallStmtLine();
			bool vaild = false;
			
			//translist�� 0�ΰ�� vaild�� false�̹Ƿ� blist[j]�� ��������.

			for(unsigned int i=0; i<translist.size();i++){				
				//Ÿ���� ��ϰ� �̸��� ȹ��
				ExeTransition* et = translist[i];
				std::string targetName = et->getName();
				std::string targetId ="";
				if(et->getTransType()==TO_BEHAVIOR){
					RFSMbehavior* tBhv = m_eTask->getBehavior(targetName);
					targetId = tBhv->getBlockID();//block�� �ٸ� block�ε� �����ϴ� behavior�� ���� ��쵵 �ִ�. (�ٽ� �����ؾ���)
				}
					
				if(et->getTransType()==TO_CONNECTOR){
					RFSMconnector* conn = m_eTask->getConnector(targetName);
					targetId = conn->getBlockID();
				}
				
				//Ÿ�ٰ� ���� ���� behavior��
				//if(nodeName.compare(targetName)==0 && nodeId.compare(targetId)==0){
				if(nodeName.compare(targetName)==0 && callLine==translist[i]->getStmtLine()){
					//���� ���� (���翡��)������ ��ȿ
					vaild = true;
				}					
			}
						
			if(!vaild){	//���� ����� ���ο� ��� ������ ���� ��� ����
				//curEBhv->removeSubBhv(pBhv->getBehaviorName());
				if(blist[j]->isMission() && !blist[j]->isCompleted()){}//�̼� ���� ���̸� destruct���� �ʴ´�.
				else enterDestruct(blist[j]);
				curNode->removeChildNode(blist[j]->getNodeName(), false, callLine); //isTaskEnd = false
				
			}					
		}		
	}
}

void TaskRunner::enterChildNodeDestruct(ExeTaskTreeNode* curNode)
{
	std::vector<ExeTaskTreeNode*>* childNodes = curNode->getAllChildNodesP();
	
	if(childNodes!=NULL && childNodes->size()>0){
		for(unsigned int i=0; i<childNodes->size();i++){
			if(childNodes->at(i) != NULL){
				enterChildNodeDestruct(childNodes->at(i));
				//if(childNodes->at(i)->isRunning()){
				//	childNodes->at(i)->stopRunning();
				//}
				enterDestruct(childNodes->at(i));
			}
		}
		//childNodes->clear();
	}	
}

/**
Behavior 1�� �̻� �Ǵ�
Behavior + Connector 1�� �̻� �Ǵ�
Connector 1���̻����� ����
*/

void TaskRunner::link_Expand(ExeTaskTreeNode* curNode, std::vector<ExeTransition*> translist)
{

//	if(curNode->runCompleted()) return;
	
	if(typeid(*curNode)==typeid(ExeConnector)){
		int conType = ((ExeConnector*)curNode)->getConType();
		if(conType == SEQEXER){	
			//seqexer -> Behavior
			unsigned int seq = ((ExeConnector*)curNode)->getCurActiveSeq();
			if(translist.size()>0 && translist[seq-1]->getTransType()==TO_BEHAVIOR){
				expand_Behavior(curNode, translist[seq-1]);
			}
			//seqexer -> Connector(seqexer|conexer)
			if(translist.size()>0 && translist[seq-1]->getTransType()==TO_CONNECTOR){			
				expand_Connector(curNode, translist[seq-1]);				
			}		
		}
		else if(conType == CONEXER){		
			for(unsigned int i=0; i<translist.size();i++){
				//Conexer -> Behavior
				if(translist[i]->getTransType()==TO_BEHAVIOR){
					expand_Behavior(curNode, translist[i]);
				}
				//Conexer -> Connector(seqexer|conexer)
				if(translist[i]->getTransType()==TO_CONNECTOR){			
					expand_Connector(curNode, translist[i]);				
				}		
			}
		}
	}		
	else if(typeid(*curNode)==typeid(ExeTask) || typeid(*curNode)==typeid(ExeBehavior)){
		//���� ��尡 ExeBehavior�̸� translist�� �Ѱ��̴�. 
		for(unsigned int i=0; i<translist.size();i++){
			//Behavior -> Behavior
			if(translist[i]->getTransType()==TO_BEHAVIOR){
				expand_Behavior(curNode, translist[i]);
			}
			//Behavior -> Connector
			if(translist[i]->getTransType()==TO_CONNECTOR){			
				expand_Connector(curNode, translist[i]);				
			}
			//Behavior -> State
			if(translist[i]->getTransType()==TO_STATE){
				//���� ���̰� state�̸� �̰� ����.(������ �Ľ̿��� ������ ã�ƾ� �Ѵ�.)
			}
		}
	}

	/*
	//Behavior�� ��� ~> �����ڸ� ����� �� �ֱ� ������.. 
	if(typeid(*curNode)==typeid(ExeBehavior)){
		std::string target = translist[0]->getName();
		int line = translist[0]->getStmtLine();
		ExeTaskTreeNode* childnode = curNode->getChildNode(target, line);
		if(childnode != NULL && childnode->isCompleted()){
			enterDestruct(childnode);
			curNode->removeChildNode(target, false, line); //�Ϸ�� ��带 ����
			std::string nextTarget = translist[0]->getNextTargetState();
			if(nextTarget != ""){//�Ϸ��� õ�̰� �����ϸ� �̵�
				((ExeBehavior*)curNode)->setNextTarget(nextTarget);
			}
			
			//else if(typeid(*curNode)==typeid(ExeTask)){
			//	curNode->setCompleted();
			//}
		}
	}
	*/

	/*else if(typeid(*curNode)==typeid(ExeConnector) || typeid(*curNode)==typeid(ExeTask)){
		std::vector<ExeTaskTreeNode*> children = curNode->getAllChildNodes();
		for(unsigned int i=0; i<children.size(); i++){
			if(children[i]->isCompleted()) 
				enterDestruct(children[i]);
		}
	}*/
	
	
	

	if(typeid(*curNode)==typeid(ExeBehavior)){
		std::string target = translist[0]->getName();
		int line = translist[0]->getStmtLine();
		ExeTaskTreeNode* childnode = curNode->getChildNode(target, line);
		if(childnode != NULL && childnode->isCompleted()){
			curNode->removeChildNode(target, false, line); //�Ϸ�� ��带 ����
			std::string nextTarget = translist[0]->getNextTargetState();
			if(nextTarget != ""){//�Ϸ��� õ�̰� �����ϸ� �̵�
				((ExeBehavior*)curNode)->setNextTarget(nextTarget);
			}
		}
	}
	else{
		if(curNode->isCompleted()) 
			runDestructorInExeNode(curNode);
	}
}

void TaskRunner::runDestructorInExeNode(ExeTaskTreeNode* node)
{
	if(node == NULL) return;
	
	if(node->isCompleted()) enterDestruct(node);
	else{
	
	//if(typeid(*node)==typeid(ExeConnector)){
		std::vector<ExeTaskTreeNode*> children = node->getAllChildNodes();
		for(unsigned int i=0; i<children.size(); i++){
			runDestructorInExeNode(children[i]);
		}
	//}

	}
}


/**
���⼭�� curNode�� �ƴ϶� linkto�� connector��� ���̴�. 
*/
void TaskRunner::expand_Connector(ExeTaskTreeNode* curNode, ExeTransition* linkto)
{
	int			calledLine	= linkto->getStmtLine();
	std::string target		= linkto->getName();
	ValueMap*	pas			= linkto->getParameters();
	ValueMap	params;

	if(pas != NULL) params = *pas;

	//case 1: ������
	ExeConnector* subExeCon = NULL;
	if(curNode->isChildExist(target, calledLine)){
		subExeCon = (ExeConnector*)(curNode->getChildNode(target, calledLine));
	}
	else{
		std::string key = target + int2str(calledLine);
		subExeCon = (ExeConnector*)(m_eTask->getMissionNode(key));
	}

	if(subExeCon != NULL){
		RFSMconnector* rfsmCon		= m_eTask->getConnector(subExeCon->getNodeName());	
		RFSMStmtBlock* withsblock	= rfsmCon->getRunBlocks();
		
		if(subExeCon != NULL){
/*			
			//���⼭ ������ runblock�� ���ؼ� decisionBlock�� ����� translist�� ���ͼ� ������
			enterConnector(subExeCon, params);
			
			//1.1 connector(linkto)�� ����� ��� ��� ȹ��
			int conType = subExeCon->getConType();
			int joinType = subExeCon->getJoinType();		
			std::vector<ExeTaskTreeNode*> cNodes = subExeCon->getAllChildNodes();		
			bool iscompleted = false;
			if(subExeCon->isRunnedAfterConstBL() && cNodes.size()<1){
				//connector�� ���� ��尡 ������
				curNode->setCompleted(true);
			}		
			else if(conType == CONEXER && joinType==AND_CONNECTOR){
				iscompleted = true;
				for(unsigned int i=0; i<cNodes.size();i++){
					std::string sub = cNodes[i]->getNodeName();
					int callstmtline = cNodes[i]->getCallStmtLine();
					ExeTaskTreeNode* childnode = subExeCon->getChildNode(sub, callstmtline);
					if(childnode != NULL && !childnode->isCompleted()){
						iscompleted = false;
						break;
					}
				}
				if(iscompleted){
					//�Ϸ�Ǹ�
					subExeCon->setCompleted(true);
printf("%s: an and-join is completed\n", subExeCon->getNodeName().c_str());
				}
			}
			else if(conType == CONEXER && joinType==OR_CONNECTOR){
				for(unsigned int i=0; i<cNodes.size();i++){
					std::string sub = cNodes[i]->getNodeName();
					int callstmtline = cNodes[i]->getCallStmtLine();
					ExeTaskTreeNode* childnode = subExeCon->getChildNode(sub, callstmtline);
					if(childnode != NULL && childnode->isCompleted()){
						iscompleted = true;					
						subExeCon->stopRunning();//���� ����						
						enterChildNodeDestruct(subExeCon);
						subExeCon->removeAllChildNodes(false); //isTaskEnd=false
						break;
					}
				}
				if(iscompleted){
					//�Ϸ�Ǹ�
					subExeCon->setCompleted(true);
printf("%s: an or-join is completed\n", subExeCon->getNodeName().c_str());
				}
			}
			else if(conType == SEQEXER){
				unsigned int seq = subExeCon->getCurActiveSeq(); 
				std::string sub	= cNodes[seq-1]->getNodeName();
				int callstmtline = cNodes[seq-1]->getCallStmtLine();
				ExeTaskTreeNode* childnode = subExeCon->getChildNode(sub, callstmtline);
				if(childnode != NULL && childnode->isCompleted()){
					//��ü �Ϸ�
					if(subExeCon->getCurActiveSeq() == subExeCon->getNumOfRunBlocks()){
						subExeCon->setCompleted(true);
printf("%s: an seqexer is completed\n", subExeCon->getNodeName().c_str());
					}
					//�߰���� �Ϸ�
					else{
						subExeCon->setCurActiveSeq(seq+1);
					}
				}
			}

			if(iscompleted)
				onBreak(curNode, withsblock, withsblock->getBlockID());
			//if(iscompleted && isBreakline(curNode, withsblock->getFileName(), withsblock->getEOL())) {
				//1. ���� ���̺� �˻��� ���� ���� ����. ���� ���̺� blockpath�� ���̵�..�̿�
			//	curNode->sendDebugMsg(m_eTask->getTask()->getName(), withsblock->getEOL(), withsblock->getBlockPath());
				//2. ��� 
			//	if(m_exeMode == DEBUG) waitAtBreakline(curNode, withsblock);
			//}
*/
			updateConnectorStatus(curNode, subExeCon);

		}//if(exeCon != NULL)	
	}

	//case 2: ó�� ����
	else{
		//2.1 ExeConnector����
		RFSMconnector* rfsmCon = m_eTask->getConnector(target);	
		int conType	= rfsmCon->getConType();
		int runType = rfsmCon->getRunType();
		int joinType = rfsmCon->getJoinType();

		ExeConnector* newConn = NULL;
		bool isMission = rfsmCon->isMission();
		bool reload = false;
		std::string key = target + int2str(calledLine);
		
		if(isMission){
			newConn = (ExeConnector*)(m_eTask->getMissionNode(key));
		}

		if(newConn == NULL){ //else�� �ϸ� �ȵȴ�.
			int prd = linkto->getPeriod();
			if(prd < 0) prd = 10;
			newConn = new ExeConnector(m_eTask->getRemoteCLI(), m_logFile, curNode, target, rfsmCon->getNameLine(), rfsmCon->getBlockID(), rfsmCon->getFileName(), rfsmCon->getSynchIdMap(), calledLine, prd);
			newConn->setConType(conType);
			newConn->setRunType(runType); //connector�� ���ԵǾ� �ִ� behaviore���� ���� Ÿ��
			newConn->setJoinType(joinType);
			newConn->setExeTask(m_eTask);

			if(rfsmCon->isMission()){ 
				newConn->setAsMission();
				m_eTask->setMissionNode(key, newConn);
			}
			curNode->addChild(newConn);
		}
		else{
			newConn->setParent(curNode);
			reload = true;
		}

		enterConnector(newConn, params);
	}
	
}

void TaskRunner::expand_Behavior(ExeTaskTreeNode* curNode, ExeTransition* moveto)
{
	//�Ķ���� ����
	int			calledline	= moveto->getStmtLine();
	std::string target		= moveto->getName();
	ValueMap*	pas			= moveto->getParameters();
	ValueMap	params;

	if(pas != NULL) params = *pas;
	int runType = curNode->getRunType();


	ExeBehavior* existingSubBhv = NULL;
	if(curNode->isChildExist(target, calledline)){
		existingSubBhv = (ExeBehavior*)(curNode->getChildNode(target, calledline));
	}
	else{
		std::string key = target + int2str(calledline);
		existingSubBhv = (ExeBehavior*)(m_eTask->getMissionNode(key));
	}



	//������
	//if(curNode->isChildExist(target, calledline)){//�̹� subtask�� ������ ���
	if(existingSubBhv != NULL){//�̹� subtask�� ������ ���
		
//Thread�� ���� ���ε� �ٽ� ������ ���(connector�� construct���� �� ���� ��� ������ �� �κ� �ʿ�)
//if(typeid(*curNode)==typeid(ExeConnector)) return; 

		//ExeBehavior* existingSubBhv = curEBhv->getSubBhvOfCurState(target);

		//ExeBehavior* existingSubBhv = NULL;
		//ExeTaskTreeNode* node = curNode->getChildNode(target, calledline);
		//if(node != NULL) existingSubBhv = (ExeBehavior*)node;

		//1. behavior->behavior
		if(typeid(*curNode)==typeid(ExeBehavior)){//behavior���� sub behavior�� nothread�� ����
			enterBehavior(existingSubBhv, params);				
		}
		//2. connector->behavior
		else if(typeid(*curNode)==typeid(ExeConnector)){
			//else if(runType == SYNCHRO || runType == ASYNCHRO){//���� ������ thread�� ����							
			existingSubBhv->setCurParam(params);			
		}
		//3. task->behavior
		else if(typeid(*curNode)==typeid(ExeTask)){
			enterBehavior(existingSubBhv, params);		
		}

		//goal state�� ���� ���� üũ
		if(existingSubBhv->isOnGoalState() && existingSubBhv->isEntryExecuted()) 
			existingSubBhv->setCompleted(true);
		
	}
	//���ο� ����
	else{
		//���̵� bhvȹ��
		RFSMbehavior* newSubBhv = m_eTask->getBehavior(target);
		//initial		
		std::string isname = newSubBhv->getInitialStateName();
		//goal
		std::vector<std::string> gsnames = newSubBhv->getGoalStateNames();
		//run Ÿ��
		int r = runType;
		if(typeid(*curNode)==typeid(ExeBehavior)) r = NOTHREAD;
	
		ExeBehavior* exeBhv = NULL;
		bool isMission = newSubBhv->isMission();
		bool reload = false;
		std::string key = target + int2str(calledline);
		
		if(isMission){
			exeBhv = (ExeBehavior*)(m_eTask->getMissionNode(key));
		}

		if(exeBhv != NULL){ //else�� �ϸ� �ȵȴ�.
			exeBhv->setParent(curNode);
			reload = true;
		}
		else{ 
			int prd = moveto->getPeriod();
			if(prd < 0) prd = 10;
			exeBhv = new ExeBehavior(m_eTask->getRemoteCLI(), m_logFile, curNode, target, newSubBhv->getNameLine(), gsnames, newSubBhv->getBlockID(), newSubBhv->getFileName(), r, calledline, prd); 
			exeBhv->setExeTask(m_eTask);
			//exeBhv->removeAllSubBhvList();
			exeBhv->setCurState(isname);
			if(newSubBhv->isMission()){
				exeBhv->setAsMission();
				m_eTask->setMissionNode(key, exeBhv);
			}
		}
		
		//���� ���� bhv�� �ڽ� bhv�� ������ ���� bhv ����
		curNode->addChild(exeBhv); 

		//�½�ũ ����
		if(r==SYNCHRO || r == ASYNCHRO){
			if(reload){ 
				exeBhv->resume();
			}
			else{
				//moveto->execute(exeBhv, m_eTask);
				exeBhv->setCurParam(params);
				exeBhv->execute();
			}
		}
		else{
			//�θ��尡 Behavior�̸� RUN_NO_THREAD�� �����
			if(m_traceThreadON)
				LOG_TRACE(m_log, " (TaskRunner:ExeTaskTreeNode<%s>, enclosed in <%s>) is created", exeBhv->getNodeName().c_str(), curNode->getNodeName().c_str());

			enterBehavior(exeBhv, params);			
		}
		//delete exeTask;
	}
}


/**
Action Block�� ���Ե� If��
*/
void TaskRunner::enterIfStmt(RFSMifStmt* ifstmt, ExeTaskTreeNode* etask)
{
	RFSMboolable* cond = ifstmt->getCondition();

	bool result = evalCondition(cond, etask, ifstmt->getEndLine());

	if(result){
		std::vector<RFSMstmt*>* tstmts = ifstmt->getTrueStmts();
		enterActionBlock(tstmts, etask);
	}
	else{
		std::vector<RFSMstmt*>* fstmts = ifstmt->getFalseStmts();
		enterActionBlock(fstmts, etask);
	}
}

void TaskRunner::initActionBlock(std::vector<RFSMstmt*>* stmts)
{
	if(stmts != NULL && stmts->size()>0){
		for(unsigned int i=0 ; i<stmts->size(); i++){
			RFSMstmt* stmt = (*stmts)[i];
			stmt->setNotPerformed();
		}
	}
}

/**
@isSeq This parameter stands for a actionBlock to be executed sequentially or not
*/

bool TaskRunner::enterActionBlock(std::vector<RFSMstmt*>* stmts, ExeTaskTreeNode* ebhv)
{
	ExeTaskTreeNode* parent = ebhv->getConnectorParent();

	ebhv->setActBlockExecuted(false);
	ebhv->setActBlockSelected(true);	
//	if(parent != NULL){
//printf("%s: Wait for action blocks selected\n", ebhv->getNodeName().c_str());
//		while(!parent->isActBlockSelected() && !m_eTask->isStopped()){
//			thread_sleep(5);
//		}
//	}

	if(stmts != NULL && stmts->size()>0){
		for(unsigned int i=0 ; i<stmts->size(); i++){
			
			RFSMstmt* stmt = (*stmts)[i];

			if(m_exeMode == MONITOR && m_suspend){
				pthread_mutex_lock(&m_suspend_mu);
				printf("Monitor-Suspend(line:%d, thread:%d)\n", stmt->getEndLine(), ebhv->getThreadNum());
				pthread_cond_wait(&m_suspend_cond, &m_suspend_mu);
				printf("Monitor-Resume(line:%d, thread:%d)\n", stmt->getEndLine(), ebhv->getThreadNum());
				pthread_mutex_unlock(&m_suspend_mu);
			}

			//RFSMblockStmt�� ���� �˻�� �ݵ�� isBreakline���� ���� �;� �Ѵ�. 
			//����� �� '}'�� ��ġ�� RFSMblockStmt�� EOL�̱� ������ ��ϳ��� stmt�� �� ����ǰ� ��� ���� �극��ũ �Ǿ�� �Ѵ�.
			if(stmt->getClassName()==CLS_RFSMblockStmt){
				RFSMStmtBlock* bstmt = (RFSMStmtBlock*)stmt;
				enterActionBlock(bstmt->getStmtBlock() , ebhv);
				stmt->setPerformed();
			}

			onBreak(ebhv, stmt, stmt->getBlockID());
			//if(isBreakline(ebhv, stmt->getFileName(), stmt->getEOL())) {
				//std::string temp = int2str(stmt->getEOL());
				//1. ���� ���̺� �˻��� ���� ���� ����. ���� ���̺� blockpath�� ���̵�..�̿�
				//ebhv->sendDebugMsg(m_eTask->getTask()->getName(), stmt->getEOL(), stmt->getBlockPath());
				//2. ��� 
				//if(m_exeMode == DEBUG) waitAtBreakline(ebhv, stmt);
			//}

			if(stmt->getClassName().compare(CLS_RFSMsysSynch)==0){
				RFSMsysSynch* rfsmsyn = (RFSMsysSynch*)stmt;
				RFSMvalue* t = rfsmsyn->getValue();
				RFSMvalueEval eval = evalValue(t, ebhv, rfsmsyn->getEndLine());	
/*		
				//----------------Begin(synch)------------------//
				int synchid = eval.getIntValue();
				while(m_eTask->isLocked_Synch(synchid) && !m_eTask->isStopped()){	//���� ���� ���� ��, �� �Էµ� ��
					thread_sleep(1);
				}
				m_eTask->waitingCntPlus(synchid);
				ebhv->notifyWaitingID(synchid);			//1. SYNCH���� ���� �� ���� �θ��忡�� �˸�
				m_eTask->updateWaitingIDs();			//2. �ֻ��� ��忡�� ���� �����ϸ� �ȵǴ� synch id�� ã��
	bool p = true;
				while(m_eTask->isWaitingID(synchid) && !m_eTask->isStopped()){//3. ��� synch�� ���� ���		
	if(p){
		printf("%s: SYNCH(%d) Waiting\n", ebhv->getNodeName().c_str(), synchid);
		p = false;
	}
					thread_sleep(1);
				}
				m_eTask->lock_Synch(synchid);			//4. ���ý��� �ϱ��� �ش� synch id�� lock
				m_eTask->waitingCntMinus(synchid);

				if(m_eTask->isAllSynchPassed(synchid)){	//5. ���ý��� �Ǿ�� �� ��� id���� ���� ���� �Ŀ�	
		printf("All SYNCH(%d) is passed\n", synchid);
					m_eTask->unlock_Synch(synchid);
					m_eTask->removeWaitingID(synchid);
					//m_eTask->resetWaitingCnt();
				}
				//------------------End(synch)------------------//
*/
				//----------------Begin(synch)------------------//
				
				//���� �����ϸ� broadcast�ϴµ� �� ���Ŀ� wait�ϸ� �ȵȴ�. 
				if(m_eTask->isStopped()) return false;
				
				int synchid = eval.getIntValue();
				
				m_eTask->waitingCntPlus(synchid);
				//�Ʒ� notifyWaitingID�� ���ŵ��� lock�Ǿ�� �Ѵ�.
				//(�ټ� �������ص� �� ����ۿ� ����, �Ժη� �ٲ������� ġ���� ����)
				pthread_mutex_lock(m_eTask->getMutex());
				//1. ���� ����� synch id ������ ���� ����
				ebhv->notifyWaitingID(synchid);			 
				//2. synch id�� ���ؼ� ��Ӵ�� ���θ� �˱� ���� root ���� �˻�
				//(��������� ��Ȳ�� �𸣱� ����)
				m_eTask->updateWaitingID();			
				pthread_mutex_unlock(m_eTask->getMutex());

				pthread_mutex_t* mu	 = m_eTask->getSYNCHMutex(synchid);
				pthread_cond_t*	 co  = m_eTask->getSYNCHCond (synchid);
				pthread_cond_t*  co2 = m_eTask->getSYNCHCond2(synchid);	
//struct timeb current;
//ftime(&current);
//printf("%d : SYNCH-start\n", current);
			
				//pthread_cond_wait,pthread_cond_broadcast���� �׻� pthread_mutex_lock/unlock���̿� ���
				pthread_mutex_lock(mu);
				if(!m_eTask->isWaitingID(synchid)){ 
					//4.synch id ���� ����
					m_eTask->removeWaitingID(synchid);
					//5.waiting ���� ����
					pthread_cond_broadcast(co);
				}
				else if(!m_eTask->isStopped()){
					//3.���⼭ synch �Ǳ⸦ ���, �Ʒ� wait�� ���ο��� mu�� �ٽ� unlock�Ѵ�.
					pthread_cond_wait(co, mu);
				}
				pthread_mutex_unlock(mu);
	
				m_eTask->waitingCntMinus(synchid);
/*
				//synch id ���� ����
				pthread_mutex_lock(mu);
				if(m_eTask->isAllSynchPassed(synchid)){
					m_eTask->removeWaitingID(synchid);
					pthread_cond_broadcast(co2);
				}
				else if(!m_eTask->isStopped()){
					pthread_cond_wait(co2, mu);
				}
				pthread_mutex_unlock(mu);
*/				

//ftime(&current);
//printf("%d : SYNCH-end\n", current);			
				//------------------End(synch)------------------//
			}
			//if(typeid(*stmt)==typeid(RFSMasmt) ){
			else if(stmt->getClassName().compare(CLS_RFSMasmt)==0){
				RFSMasmt* asmt = (RFSMasmt*)stmt;
				int el = ((RFSMstmt*)asmt)->getEndLine(); //RFSMstmt*�� RFSMvalue* �ΰ��߿� �ϳ��� casting
				evalValue(asmt, ebhv, el);				
			}
			else if(stmt->getClassName().compare(CLS_RFSMvar)==0){
				RFSMvar* varDecl = (RFSMvar*)stmt;
				int el = ((RFSMvalue*)varDecl)->getEndLine(); //RFSMstmt*�� RFSMvalue* �ΰ��߿� �ϳ��� casting
				evalValue(varDecl, ebhv, el);		
			}
			else if(stmt->getClassName().compare(CLS_RFSMsysWaitfor)==0){
				RFSMsysWaitfor* rfsmwait = (RFSMsysWaitfor*)stmt;
				RFSMvalue* t = rfsmwait->getValue();
				RFSMvalueEval eval = evalValue(t, ebhv, rfsmwait->getEndLine());	
				//thread_sleep(eval.getIntValue());

				// timeout �ð� ����(ex 1510)
				unsigned int milliseconds = eval.getIntValue();
				timespec ts;
				
				#ifdef _WIN32
				ts.tv_sec = milliseconds / 1000;  // 1 sec
				ts.tv_nsec = (milliseconds - ts.tv_sec*1000) * 1000000; //510 *1000000(milli_sec)
				#endif
				#ifndef _WIN32
				struct timeval now;
				gettimeofday(&now, NULL);
				unsigned long usec = (now.tv_usec + milliseconds*1000);
				ts.tv_sec = now.tv_sec + (unsigned long)(usec/1000000);
				ts.tv_nsec = (usec%1000000)*1000;
				#endif
			
				pthread_mutex_lock(m_eTask->getSLEEPStmtMutex());
				pthread_cond_timedwait(m_eTask->getSLEEPStmtCond(), m_eTask->getSLEEPStmtMutex(), &ts);
				//pthread_cond_timedwait(m_eTask->getSLEEPStmtCond(), m_eTask->getSLEEPStmtMutex(), eval.getIntValue());
				pthread_mutex_unlock(m_eTask->getSLEEPStmtMutex());
				if(RES == 0) return false;
			}
			/*
			//else if(typeid(*stmt)==typeid(RFSMcallAction) ){
			else if(stmt->getClassName().compare(CLS_RFSMcallAction)==0){

				ValueMap* paramsEval = NULL;
				RFSMcallAction* action = (RFSMcallAction*)stmt;
				EList<RFSMparam*>* params = action->getParameters();
				if(params != NULL){
					paramsEval = evaluateParams(params, ebhv);
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////
				//action�� ȣ���ϱ� ������ out model���� �ݿ��Ѵ�.
				if(params != NULL && paramsEval == NULL){
					FLOG_ERROR(m_logFile, "  (RUNNING:ACTION) %s, Calling action is skipped.", action->toString().c_str());
				}
				else{
					//m_eTask->setOutModelValue();
					m_binder->callFunction(m_logFile, RFSMcall::CALL_ACTION, TYP_VOID, action->getName(), paramsEval);
				}
				////////////////////////////////////////////////////////////////////////////////////////////////
							
				return true; //Action�� ȣ���ϸ� ����Ŭ�� ��. ������ �ǹ̾��� ��..(�ϳ��� action�� ����)
			}
			*/
			//else if(typeid(*stmt)==typeid(RFSMcallFunc) ){
			else if(stmt->getClassName().compare(CLS_RFSMcallFunc)==0){

				ValueMap* paramsEval = NULL;
				RFSMcallFunc* rfunc = (RFSMcallFunc*)stmt;
				std::vector<RFSMparam*>* params = rfunc->getParameters();
				if(params != NULL){
					int el = ((RFSMvalue*)rfunc)->getEndLine(); //RFSMstmt*�� RFSMvalue* �ΰ��߿� �ϳ��� casting
					paramsEval = evalParams(params, ebhv, el);
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////
				//
				if(params != NULL && paramsEval == NULL){
					FLOG_ERROR(m_logFile, "   (RUNNING:FUNCTION) %s, Calling function/action is skipped.", rfunc->toString().c_str());
				}
				else{
					int ct = CALL_FUNC;
					if(rfunc->isAction())
						ct = CALL_ACTION;

					//m_binder->callFunction(m_logFile, ct, rfunc->getType(), rfunc->getName(), paramsEval);
					bind4Function(ebhv, ct, rfunc, paramsEval);
//printf("[EOL:RFSMcallFunc]%s(%d): %d\n", rfunc->getBlockFullName().c_str(), ebhv->getThreadID(), rfunc->getEOL());
				}
				//////////////////////////////////////////////////////////////////////////////////////////////////
			
				if(paramsEval != NULL){
					paramsEval->clear(); 
					delete paramsEval;
				}
				
				stmt->setPerformed();

				if(rfunc->isAction()){					
					return true;
				}
			}		

			//else if(typeid(*stmt)==typeid(RFSMifStmt) ){
			else if(stmt->getClassName().compare(CLS_RFSMifStmt)==0){
				RFSMifStmt* ifstmt = (RFSMifStmt*)stmt;
				enterIfStmt(ifstmt, ebhv);				
			}

			else if(stmt->getClassName().compare(CLS_RFSMsymbStmt)==0){
				//�׳� ������ a;				
			}

			
			else if(stmt->getClassName()==CLS_RFSMseqStmt){
				FLOG_TRACE(m_logFile, " SEQUENTIAL:");	
				RFSMStmtBlock* bstmt = (RFSMStmtBlock*)stmt;
				std::vector<RFSMstmt*>* withstmts = bstmt->getStmtBlock();
				if(withstmts != NULL && withstmts->size() >0){
					enterActionBlock(withstmts , ebhv);
				}				
			}
			else if(stmt->getClassName()==CLS_RFSMparStmt){
				FLOG_TRACE(m_logFile, " PARALLEL:");	
				RFSMStmtBlock* bstmt = (RFSMStmtBlock*)stmt;
				std::vector<RFSMstmt*>* withstmts = bstmt->getStmtBlock();
				if(withstmts != NULL && withstmts->size() >0){
					enterWithBlock4Par(withstmts , ebhv);
				}						
			}
			else if(stmt->getClassName()==CLS_RFSMwithStmt){//sequential�� ��� ����
				FLOG_TRACE(m_logFile, "  +THEN");	
				RFSMStmtBlock* bstmt = (RFSMStmtBlock*)stmt;
				std::vector<RFSMstmt*>* stmts = bstmt->getStmtBlock();
				if(stmts != NULL && stmts->size() >0){
					enterActionBlock(stmts , ebhv);
				}				
			}
		}
	}
	
	ebhv->setActBlockExecuted(true);

	//if(parent != NULL){
	//	while(!parent->isActBlockExecuted() && !m_eTask->isStopped()){
	//		thread_sleep(5);
	//	}
	//}
	
	ebhv->setActBlockSelected(false);

	return false;
}

void TaskRunner::enterWithBlock4Par(std::vector<RFSMstmt*>* withstmts, ExeTaskTreeNode* ebhv)
{
	std::vector<ExeActionBlock*> eablist;

	if(withstmts != NULL && withstmts->size()>0){
		for(unsigned int i=0 ; i<withstmts->size(); i++){
			RFSMstmt* stmt = (*withstmts)[i];
			if(stmt->getClassName()==CLS_RFSMwithStmt){
				FLOG_TRACE(m_logFile, "  +WITH");			
				RFSMStmtBlock* bstmt = (RFSMStmtBlock*)stmt;
				//with block���� stmt��
				std::vector<RFSMstmt*>* stmts = bstmt->getStmtBlock();
				ExeActionBlock* ablock = new ExeActionBlock(m_eTask, 5);
				eablist.push_back(ablock);
				ablock->setParameters(stmts, (ExeBehavior*)ebhv);
				ablock->execute();				
			}
		}
	}	

	//AND-join(���� �ٸ� ���� ������� �ʰ� �ִ�.)
	if(eablist.size()>0){
		for(unsigned int i=0; i<eablist.size();i++){
			ExeActionBlock* ab = eablist[i];
			ab->setIsRunning(false);
			ab->wait4join();//join
			delete ab;
		}
	}
}

ValueMap* TaskRunner::evalParams(std::vector<RFSMparam*>* params, ExeTaskTreeNode* ebhv, int line)
{
	ValueMap* paramsEval = NULL;
	if(params!= NULL && params->size()>0){
		paramsEval = new ValueMap();		
		for(unsigned int i=0; i<params->size();i++){
			RFSMparam* param = (*params)[i];
			RFSMvalue* pVal = param->getValue();
			RFSMvalueEval pValEval = evalValue(pVal, ebhv, line);
			pValEval.setIndex(i);
			if(pValEval.isNULL()){
				FLOG_ERROR(m_logFile, "   (RUNNING:PARAMETER) %s, Parameter value error", RFSMvalue::toString(pVal).c_str());
				return NULL;
			}
			//���⼭ �Ķ�����̸��� blockpath�� ���̸� �ȵȴ�. �Լ��� actionȣ�� �Ķ���͵� �ٷ�� ����
			
			paramsEval->insert(ValuePair(param->getName(), pValEval));
		}
	}

	return paramsEval;
}

RFSMvalueEval TaskRunner::evalValue(RFSMvalue* value, ExeTaskTreeNode* ebhv, int line)
{
	//��Ӱ��� ������ �ڽ��� �׻� �������� �ݵ�� else ���ϰ�

	if(typeid(*value)==typeid(RFSMmulti) ){
		RFSMmulti* bop = (RFSMmulti*)value;
		return evalBinaryOp(bop, ebhv, line);
	}

	if(typeid(*value)==typeid(RFSMadd) ){
		RFSMadd* bop = (RFSMadd*)value;
		return evalBinaryOp(bop, ebhv, line);
	}

	else if(typeid(*value)==typeid(RFSMvar) ){ //���� ����
		RFSMvar* varDecl = (RFSMvar*)value;
		RFSMvariable* lvar = varDecl->getLvalue();
		//�ʱ�ȭ ���� �ʾұ� ������ ������ RFSMvalueEval����
		RFSMvalueEval rvalEval(line, lvar->getBlockName());
		rvalEval.setType(lvar->getType());
		unsigned int varMode = lvar->getMode();
		if(varMode==G_LOCAL){
			bind4SetLocalVar(ebhv, lvar, rvalEval, line);
		}
	}

	else if(typeid(*value)==typeid(RFSMasmt) ){

		RFSMasmt* rasmt = (RFSMasmt*)value;
		RFSMvariable* lvar = rasmt->getLvalue();
		RFSMvalue* rval = rasmt->getRvalue();
		RFSMvalueEval rvalEval = evalValue(rval, ebhv, line);
		if(rvalEval.isNULL()){
			FLOG_ERROR(m_logFile, "   (RUNNING:Assignment)R-value error, Assignment, %s, is skipped.", rasmt->toString().c_str());
			return RFSMvalueEval();
		}

		//���̺� ����
		//if(lvar->isLocal()){//���������� ���
		unsigned int varMode = lvar->getMode();
		if(varMode==G_LOCAL){
			//std::string varName = lvar->getBlockPath();
			//varName.append(".").append(lvar->getName());
			//ebhv->addLocalValue(varName, rvalEval);
			bind4SetLocalVar(ebhv, lvar, rvalEval, line);
		}
		else if(varMode==G_GVAR){
			//m_binder->setGlobalValue(m_logFile, lvar->getName(), rvalEval);
			bind4SetGlobalVar(ebhv, lvar, rvalEval, line);
		}
		else if(varMode==G_WVAR){
			m_eTask->setWorkerVar(lvar->getName(), rvalEval);
		}
		else if(varMode==G_VAROUT){//model���� ��� (OUTŸ��)

			////////////////////////////////////////////////////////////////////////////////////
			//���� ���̺� ����� action/function ȣ���ϱ� ������ �����Ѵ�.
			//m_eTask->addOutModelValue(lvar->getName(), rvalEval);
			//m_binder->setModelValue(m_logFile, lvar->getName(), rvalEval);
			bind4SetModelSym(ebhv, lvar, rvalEval, line);

			////////////////////////////////////////////////////////////////////////////////////
		}
//printf("[EOL:RFSMasmt]%s(%d): %d\n", rasmt->getBlockFullName().c_str(), ebhv->getThreadID(), rasmt->getEOL());
		return rvalEval;
	}
	else if(typeid(*value)==typeid(RFSMcallFunc) ){

		ValueMap* paramsEval = NULL;
		RFSMcallFunc* rfunc = (RFSMcallFunc*)value;
		std::vector<RFSMparam*>* params = rfunc->getParameters();
		if(params != NULL){//�Ű������� �ִ°��
			paramsEval = evalParams(params, ebhv, line);
		}
		/////////////////////////////////////////////////////////////////////////////////////////
		//
		if(params != NULL && paramsEval == NULL){
			FLOG_ERROR(m_logFile, "   (RUNNING:FUNCTION) %s, Calling function is skipped.", rfunc->toString().c_str());
			return RFSMvalueEval();
		}
		else{		
			//RFSMvalueEval eval = m_binder->callFunction(m_logFile, RFSMcall::CALL_FUNC, rfunc->getType(), rfunc->getName(), paramsEval);
			RFSMvalueEval eval = bind4Function(ebhv, CALL_FUNC, rfunc, paramsEval);
//printf("[EOL:RFSMcallFunc]%s(%d): %d\n", rfunc->getBlockFullName().c_str(), ebhv->getThreadID(), rfunc->getEOL());
			//if(paramsEval != NULL) delete paramsEval;
			delete paramsEval;
			return eval;
		}
		///////////////////////////////////////////////////////////////////////////////////////
	}

	else if(typeid(*value)==typeid(RFSMcondValue) ){

		RFSMcondValue* rcondVal = (RFSMcondValue*)value;
		RFSMboolable* cond = rcondVal->getCondition();
		bool result = evalCondition(cond, ebhv, line);
		RFSMvalue* v = NULL;
		if(result) v = rcondVal->getTrueValue();
		else v= rcondVal->getFalseValue();
		RFSMvalueEval vEval = evalValue(v, ebhv, line);
		return vEval;
	}

	else if(typeid(*value)==typeid(RFSMliteral) ){

		RFSMliteral* rlite = (RFSMliteral*)value;
		unsigned int type = rlite->getType();
		std::string data = rlite->getData();
		RFSMvalueEval vEval(line, rlite->getBlockName());
		vEval.setLiteral(type, data);
		return vEval;
	}
	else if(typeid(*value)==typeid(RFSMsign)){

		RFSMsign* rsign = (RFSMsign*)value;
		RFSMvalueEval vEval = evalValue(rsign->getValue(), ebhv, line);
		if(rsign->hasMinus())
			vEval.setMinus();
		return vEval;
	}
	else if(typeid(*value)==typeid(RFSMvariable) ){

		RFSMvariable* rsym = (RFSMvariable*)value;
		//if(rsym->isLocal()){
		unsigned int varmode = rsym->getMode();
		if(varmode==G_LOCAL){
			//���ú����� blockpath�� �ٿ��� �˻��ؾ��Ѵ�.
			//std::string name = rsym->getBlockPath();
			//name.append(".").append(rsym->getName());
			//return ebhv->getLocalValue(name);
			return bind4GetLocalVar(ebhv, rsym, line);
		}
		else if(varmode==G_GVAR){
			//return m_binder->getGlobalValue(m_logFile, rsym->getName());
			return bind4GetGlobalVar(ebhv, rsym, line);
			
		}
		else if(varmode==G_WVAR){
			return m_eTask->getWorkerVar(rsym->getName());
		}
		else{
			////////////////////////////////////////////////////////////////////////////////////////////
			//return m_binder->getModelValue(m_logFile, rsym);
			return bind4GetModelSym(ebhv, rsym, line);
			//return getModelValue(rsym->getName());
			////////////////////////////////////////////////////////////////////////////////////////////
		}
	}
	else if(typeid(*value)==typeid(RFSMenum)){
		RFSMenum* renum = (RFSMenum*)value;
		RFSMvalueEval vEval(line, renum->getBlockName());
		vEval.setIntValue(renum->getValue());
		return vEval;
	}
	else if(typeid(*value)==typeid(RFSMsystem)){
		RFSMsystem* rsys = (RFSMsystem*)value;
		int type = rsys->getType();
		if(type == TYP_SYS_BEHAVIORTIME){
			return ebhv->getNodeExeTime(line);
		}
		else if(type == TYP_SYS_STATETIME){
			if(typeid(*ebhv)==typeid(ExeBehavior) ){
				return ((ExeBehavior*)ebhv)->getStateTime(line);
			}
		}
		else if(type == TYP_SYS_TASKTIME){
			return m_eTask->getTaskTime(line);
		}
	}

	RFSMvalueEval valEval(line, value->getBlockName());
	valEval.isNULL();
	return valEval;
}


RFSMvalueEval TaskRunner::evalBinaryOp(RFSMbinaryOp* biop, ExeTaskTreeNode* ebhv, int line)
{
	//RuntimeEH runEH;

	unsigned int op = biop->getOperator();
	RFSMvalue* left = biop->getLeftOperand();
	RFSMvalue* right = biop->getRightOperand();

	RFSMvalueEval eleft = evalValue(left, ebhv, line);
	RFSMvalueEval eright = evalValue(right, ebhv, line);

	///////////////////////////////////////////////////////////////////////
	//���� ����
	if(eleft.isNULL() == true || eright.isNULL() == true){
		//runEH.setErrorCode(RTMERR_GET_VARIABLE_VALUE);
		FLOG_ERROR(m_logFile, "   (RUNNING:Decision) %s, Value evaluation Error. Decision operation is skipped ", biop->toString().c_str());
		return RFSMvalueEval();
	}
	///////////////////////////////////////////////////////////////////////

	unsigned int eleftType = eleft.getType();
	unsigned int erightType = eright.getType();
	unsigned int type = computationType(eleftType, erightType);

	RFSMvalueEval valEval(line, left->getBlockName());
	bool valued = false;

	if(type == TYP_INT){
		int lval = eleft.getIntValue();
		int rVal = eright.getIntValue();

		if(op==OP_PLU){
			valEval.setType(TYP_INT);
			valEval.setIntValue(lval + rVal);
			valued = true;
		}

		if(op==OP_MIN){
			valEval.setType(TYP_INT);
			valEval.setIntValue(lval - rVal);
			valued = true;
		}

		if(op==OP_MUL){
			valEval.setType(TYP_INT);
			valEval.setIntValue(lval * rVal);
			valued = true;
		}

		if(op==OP_DVI){
			valEval.setType(TYP_INT);
			valEval.setIntValue(lval / rVal);
			valued = true;
		}

		if(op==OP_GRE){//�º��� ����: �º��� ũ��.
			bool result = false;
			if(lval>rVal)result = true;
			valEval.setType(TYP_BOOL);
			valEval.setBoolValue(result);
			valued = true;
		}

		if(op==OP_LES){
			bool result = false;
			if(lval<rVal)result = true;
			valEval.setType(TYP_BOOL);
			valEval.setBoolValue(result);
			valued = true;
		}

		if(op==OP_GRE_EQU){
			bool result = false;
			if(lval>=rVal)result = true;
			valEval.setType(TYP_BOOL);
			valEval.setBoolValue(result);
			valued = true;
		}

		if(op==OP_LES_EQU){
			bool result = false;
			if(lval<=rVal)result = true;
			valEval.setType(TYP_BOOL);
			valEval.setBoolValue(result);
			valued = true;
		}

		if(op==OP_EQU){
			bool result = false;
			if(lval==rVal)result = true;
			valEval.setType(TYP_BOOL);
			valEval.setBoolValue(result);
			valued = true;
		}

		if(op==OP_NOT_EQU){
			bool result = false;
			if(lval!=rVal)result = true;
			valEval.setType(TYP_BOOL);
			valEval.setBoolValue(result);
			valued = true;
		}
	}
	else if(type == TYP_FLOAT){
		float lval = eleft.getFloatValue();
		float rVal = eright.getFloatValue();

		if(op==OP_PLU){
			valEval.setType(TYP_FLOAT);
			valEval.setFloatValue(lval + rVal);
			valued = true;

		}
		if(op==OP_MIN){
			valEval.setType(TYP_FLOAT);
			valEval.setFloatValue(lval - rVal);
			valued = true;

		}
		if(op==OP_MUL){
			valEval.setType(TYP_FLOAT);
			valEval.setFloatValue(lval * rVal);
			valued = true;

		}
		if(op==OP_DVI){
			valEval.setType(TYP_FLOAT);
			valEval.setFloatValue(lval / rVal);
			valued = true;

		}
		if(op==OP_GRE){//�º��� ����: �º��� ũ��.
			bool result = false;
			if(lval>rVal)result = true;
			valEval.setType(TYP_BOOL);
			valEval.setBoolValue(result);
			valued = true;

		}
		if(op==OP_LES){
			bool result = false;
			if(lval<rVal)result = true;
			valEval.setType(TYP_BOOL);
			valEval.setBoolValue(result);
			valued = true;

		}
		if(op==OP_GRE_EQU){
			bool result = false;
			if(lval>=rVal)result = true;
			valEval.setType(TYP_BOOL);
			valEval.setBoolValue(result);
			valued = true;

		}
		if(op==OP_LES_EQU){
			bool result = false;
			if(lval<=rVal)result = true;
			valEval.setType(TYP_BOOL);
			valEval.setBoolValue(result);
			valued = true;

		}
		if(op==OP_EQU){
			bool result = false;
			if(lval==rVal)result = true;
			valEval.setType(TYP_BOOL);
			valEval.setBoolValue(result);
			valued = true;
		}
		if(op==OP_NOT_EQU){
			bool result = false;
			if(lval!=rVal)result = true;
			valEval.setType(TYP_BOOL);
			valEval.setBoolValue(result);
			valued = true;
		}

	}
	else if(type == TYP_STRING){
		std::string lval = eleft.getStrValue();
		std::string rVal = eright.getStrValue();

		if(op==OP_PLU){
			std::string result = lval.append(rVal);
			valEval.setType(TYP_STRING);
			valEval.setStrValue(result);
			valued = true;
		}

		if(op==OP_EQU){
			bool result = false;
			if(lval.compare(rVal)==0) result= true;
			valEval.setType(TYP_BOOL);
			valEval.setBoolValue(result);
			valued = true;
		}

		if(op==OP_NOT_EQU){
			bool result = false;
			if(lval.compare(rVal)!=0) result= true;
			valEval.setType(TYP_BOOL);
			valEval.setBoolValue(result);
			valued = true;
		}

	}
	else if(type == TYP_BOOL){
		//�̰� �����̴�.
	}
	else if(type == TYP_NOT_PRIMITIVE){
	}
	else if(type == TYP_UNCOMPATIBLE){
	}

	//delete eleft;
	//delete eright;
	if(!valued){
		//delete valEval;
		return RFSMvalueEval();
	}

	return valEval;
}

/**
* �߰��� ���� �������� ������ false�� �����Ѵ�.
* �׷���.. ���� �ʴ´�. ���� ����� ������ �����Ѵ�.
*/
bool TaskRunner::evalCondition(RFSMboolable* cond, ExeTaskTreeNode* ebhv, int line)
{
	bool eval = false;
	std::string stn = "";

	if(cond->getClassName().compare(CLS_RFSMor)==0){
		RFSMor* orp = (RFSMor*) cond;
		bool result = false;
		std::vector<RFSMboolable*>* boolexprs = orp->getBoolExprs();
		if(boolexprs != NULL && boolexprs->size()>0){
			for(unsigned int i=0;i<boolexprs->size();i++){
				RFSMboolable* boolexpr = (*boolexprs)[i];
				result = evalCondition(boolexpr, ebhv, line);
				if(result == true){
					eval= true;
					break;
				}
			}
		}
		stn.append(RFSMboolable::toString(orp)).append(" ");
	}

	else if(cond->getClassName().compare(CLS_RFSMand)==0){
		RFSMand* andp = (RFSMand*) cond;
		bool result = true;
		std::vector<RFSMboolable*>* boolexprs = andp->getBoolExprs();
		if(boolexprs != NULL && boolexprs->size()>0){
			eval = true;//������ ��
			for(unsigned int i=0;i<boolexprs->size();i++){
				RFSMboolable* boolexpr = (*boolexprs)[i];
				result = evalCondition(boolexpr, ebhv, line);
				if(result == false) {
					eval = false;
					break;
				}
			}
		}
		stn.append(RFSMboolable::toString(andp)).append(" ");
	}

	else if(cond->getClassName().compare(CLS_RFSMcallFunc)==0){
		RFSMcallFunc* func = (RFSMcallFunc*)cond;
		RFSMvalueEval veval = evalValue(func, ebhv, line);
		eval = veval.getBoolValue();
		stn.append(RFSMboolable::toString(func));
//		delete veval;
	}

	else if(cond->getClassName().compare(CLS_RFSMRelational)==0){
		RFSMRelational* rel = (RFSMRelational*)cond;
		RFSMvalueEval veval = evalBinaryOp(rel, ebhv, line);
		eval = veval.getBoolValue();
		stn.append(RFSMboolable::toString(rel));
	//	delete veval;
	}

	else if(cond->getClassName().compare(CLS_RFSMliteral)==0){
		RFSMliteral* lite = (RFSMliteral*)cond;
		RFSMvalueEval veval =  evalValue(lite, ebhv, line);
		eval = veval.getBoolValue();
		stn.append(RFSMboolable::toString(lite));
	//	delete veval;
	}

	else if(cond->getClassName().compare(CLS_RFSMvariable)==0){
		RFSMvariable* sym = (RFSMvariable*)cond;
		RFSMvalueEval veval =  evalValue(sym, ebhv, line);
		eval = veval.getBoolValue();
		stn.append(RFSMboolable::toString(sym));
	//	delete veval;
	}

	if(cond->hasNot())
		return !eval;

	if(eval){
		FLOG_TRACE(m_logFile, "   (RUNNING:Decision) *TRUE*, %s ", stn.c_str());
	}
	else{
		FLOG_TRACE(m_logFile, "   (RUNNING:Decision) FALSE, %s ", stn.c_str());
	}

	return eval;
}

bool TaskRunner::onBreak(ExeTaskTreeNode* node, RFSMBlockInfo* bp, std::string blockpath)
{
	if(isBreakline(node, bp->getFileName(), bp->getEndLine())) {
		//1. ���� ���̺� �˻��� ���� ���� ����. ���� ���̺� blockpath�� ���̵�..�̿�
		node->sendDebugMsg(&m_modelTAB, m_eTask->getTask()->getName(), bp->getEndLine(), blockpath);
		//2. ��� 
		if(m_exeMode == DEBUG){ 
			waitAtBreakline(node); //break������ �����Ǳ⸦ ��ٸ���.
			return true;
		}
	}
	return false;
}

bool TaskRunner::isBreakline(ExeTaskTreeNode* node, std::string name, int line)
{
	pthread_mutex_lock(m_eTask->getMutex());

	int threadid = node->getThreadNum();
	if(threadid==0)threadid = node->getParentThreadID();

	if(m_exeMode == DEBUG || m_exeMode == MONITOR){
		
		//�ʱ�ȭ
		bool isbreakpoint = false;
		bool toNextBP	= false;
		bool toNextStep = false;

		std::map<int, bool>::iterator it3 = m_moveNext_BP.find(threadid);
		if(it3 != m_moveNext_BP.end()){
			toNextBP = it3->second;
		}

		std::map<int, bool>::iterator it4 = m_moveNext_STEP.find(threadid);
		if(it4 != m_moveNext_STEP.end()){
			toNextStep = it4->second;
		}

		//1.����� ������� �Ǵ�	
		if(toNextBP){			
			isbreakpoint = DbgTable::findByFile(name, line);//��Ȯ�� breakline�� ��ġ�ϴ��� �˻�.
		}
		else if(m_exeMode == DEBUG && toNextStep){//monitor�� ���� ���� ���� �̵��� ���۾��Ѵ�.
			isbreakpoint = true;
		}
		pthread_mutex_unlock(m_eTask->getMutex());

		return isbreakpoint;
	}

	pthread_mutex_unlock(m_eTask->getMutex());
	return false;
}

/*
pthread_mutex_t* TaskRunner::getBPMutex(int threadid)
{
	std::map<int, pthread_mutex_t>::iterator mapiter;
	mapiter = m_BP_wakeup_mus.find(threadid);
	if(mapiter != m_BP_wakeup_mus.end()){
		return &(mapiter->second);
	}
	return NULL;
}
*/

void TaskRunner::waitAtBreakline(ExeTaskTreeNode* node)
{
	if(!m_eTask->isStopped()){
		int threadid = node->getCreatedThreadID();
		pthread_mutex_t* bp_mu	= getBPMutex(threadid);
		pthread_cond_t*	 bp_co	= getBPCond (threadid);

		//2.1 ������ ����ڰ� Ŭ���� ����� �Է¹ޱ� ���� ������ �Ѵ�.(������ ������ �� ��ġ �߿�)
		//m_moveNextStep	= false;
		//m_moveNextBP	= false;

		//2.2 ���
		pthread_mutex_lock(bp_mu);		
		//node->sendDebugMsg(m_eTask->getTask()->getName(), stmt->getEOL(), stmt->getBlockPath());
		pthread_cond_wait(bp_co, bp_mu);
		pthread_mutex_unlock(bp_mu);

		//2.2 ���
		//while(!s_moveNextStep && !s_moveNextBP && !m_eTask->isStopped()){
		//	thread_sleep(100);
		//}	
	}
}

void TaskRunner::wakeupAllBPwait()
{
	std::map<int, pthread_cond_t>::iterator it;
	for(it=m_BP_wakeup_conds.begin(); it!=m_BP_wakeup_conds.end(); it++){
		int threadid = it->first;
		pthread_cond_t bp_co = it->second;
		pthread_mutex_t* bp_mu	= getBPMutex(threadid);

		pthread_mutex_lock(bp_mu);
		pthread_cond_signal(&bp_co);
		pthread_mutex_unlock(bp_mu);
	}
}

pthread_cond_t* TaskRunner::getBPCond(int threadid)
{
	std::map<int, pthread_cond_t>::iterator	mapiter;
	mapiter = m_BP_wakeup_conds.find(threadid);
	if(mapiter != m_BP_wakeup_conds.end()){
		return &(mapiter->second);
	}
	return NULL;
}

pthread_mutex_t* TaskRunner::getBPMutex(int threadid)
{
	std::map<int, pthread_mutex_t>::iterator mapiter;
	mapiter = m_BP_wakeup_mus.find(threadid);
	if(mapiter != m_BP_wakeup_mus.end()){
		return &(mapiter->second);
	}
	return NULL;
}

int TaskRunner::addCondMu4BP(int threadid)
{
	pthread_mutex_lock(m_eTask->getMutex());
	int result = 0;
	std::map<int, pthread_cond_t>::iterator it = m_BP_wakeup_conds.find(threadid);
	if(it != m_BP_wakeup_conds.end()) result = -1;
	else{	
		pthread_cond_t bpcond;
		pthread_cond_init(&bpcond, NULL);
		m_BP_wakeup_conds.insert(std::pair<int, pthread_cond_t>(threadid, bpcond));
	}

	std::map<int, pthread_mutex_t>::iterator it2 = m_BP_wakeup_mus.find(threadid);
	if(it2 != m_BP_wakeup_mus.end()) result = -1;
	else{
		pthread_mutex_t bpmu;
		pthread_mutex_init(&bpmu, NULL);
		m_BP_wakeup_mus.insert(std::pair<int, pthread_mutex_t>(threadid, bpmu));
	}

	std::map<int, bool>::iterator it3 = m_moveNext_BP.find(threadid);
	if(it3 != m_moveNext_BP.end()) result = -1;
	else{
		m_moveNext_BP.insert(std::pair<int, bool>(threadid, true));
	}

	std::map<int, bool>::iterator it4 = m_moveNext_STEP.find(threadid);
	if(it4 != m_moveNext_STEP.end()) result = -1;
	else{
		m_moveNext_STEP.insert(std::pair<int, bool>(threadid, false));
	}
	pthread_mutex_unlock(m_eTask->getMutex());

	return result;
}

int TaskRunner::delCondMu4BP(int threadid)
{
	pthread_mutex_lock(m_eTask->getMutex());
	int result = 0;
	std::map<int, pthread_cond_t>::iterator it = m_BP_wakeup_conds.find(threadid);
	if(it != m_BP_wakeup_conds.end()){
		pthread_cond_destroy(&(it->second));
		m_BP_wakeup_conds.erase(it);
	}
	else result = -1;

	std::map<int, pthread_mutex_t>::iterator it2 = m_BP_wakeup_mus.find(threadid);
	if(it2 != m_BP_wakeup_mus.end()){
		pthread_mutex_destroy(&(it2->second));
		m_BP_wakeup_mus.erase(it2);
	}
	else result = -1;

	std::map<int, bool>::iterator it3 = m_moveNext_BP.find(threadid);
	if(it3 != m_moveNext_BP.end()){
		m_moveNext_BP.erase(it3);
	}
	else result = -1;


	std::map<int, bool>::iterator it4 = m_moveNext_STEP.find(threadid);
	if(it4 != m_moveNext_STEP.end()){
		m_moveNext_STEP.erase(it4);
	}
	else result = -1;
	pthread_mutex_unlock(m_eTask->getMutex());

	return result;
}

int TaskRunner::setMove2NextBP_On(int threadid)
{
	//pthread_mutex_lock(&m_runner->m_BP_wakeup_mu);
	//m_runner->s_moveNextBP = true;
	//pthread_cond_broadcast(&m_runner->m_BP_wakeup_cond);
	//pthread_mutex_unlock(&m_runner->m_BP_wakeup_mu);
	int result = 0;

	pthread_mutex_t* bp_mu	= getBPMutex(threadid);
	pthread_cond_t*	 bp_co	= getBPCond (threadid);

	if(bp_mu == NULL || bp_co ==NULL) return WRONG_TH_ID; 

	pthread_mutex_lock(bp_mu);
	//m_moveNextBP	= true;
	//m_moveNextStep	= false;
	std::map<int, bool>::iterator it3 = m_moveNext_BP.find(threadid);
	if(it3 != m_moveNext_BP.end()){
		it3->second = true;
	}
	else result = -1;

	std::map<int, bool>::iterator it4 = m_moveNext_STEP.find(threadid);
	if(it4 != m_moveNext_STEP.end()){
		it4->second = false;
	}
	else result = -1;
	pthread_cond_signal(bp_co);
	pthread_mutex_unlock(bp_mu);

	return 0;
}

int TaskRunner::setMove2NextStep_On(int threadid)
{
	//pthread_mutex_lock(&m_runner->m_BP_wakeup_mu);
	//m_runner->s_moveNextStep = true;
	//pthread_cond_broadcast(&m_runner->m_BP_wakeup_cond);
	//pthread_mutex_unlock(&m_runner->m_BP_wakeup_mu);
	int result = 0;

	pthread_mutex_t* bp_mu	= getBPMutex(threadid);
	pthread_cond_t*	 bp_co	= getBPCond (threadid);

	if(bp_mu == NULL || bp_co ==NULL) return WRONG_TH_ID;  

	pthread_mutex_lock(bp_mu);
	//m_moveNextBP	= false;
	//m_moveNextStep	= true;
	std::map<int, bool>::iterator it3 = m_moveNext_BP.find(threadid);
	if(it3 != m_moveNext_BP.end()){
		it3->second = false;
	}
	else result = -1;

	std::map<int, bool>::iterator it4 = m_moveNext_STEP.find(threadid);
	if(it4 != m_moveNext_STEP.end()){
		it4->second = true;
	}
	else result = -1;

	pthread_cond_signal(bp_co);
	pthread_mutex_unlock(bp_mu);

	return result;
}

int	TaskRunner::setSuspendMonitor()
{
	pthread_mutex_lock(&m_suspend_mu);
	m_suspend = true;
	pthread_mutex_unlock(&m_suspend_mu);
	return 0;
}

int	TaskRunner::setResumeMonitor()
{
	pthread_mutex_lock(&m_suspend_mu);
	pthread_cond_broadcast(&m_suspend_cond);
	pthread_mutex_unlock(&m_suspend_mu);
	m_suspend = false;
	return 0;
}

void TaskRunner::setAsMonitorMode()
{
	m_exeMode		= MONITOR;
	m_suspend		= false;
	//m_moveNextBP	= true;
	//m_moveNextStep	= false;
}

void TaskRunner::setAsDebugMode(void)
{
	m_exeMode		= DEBUG;
	//m_moveNextBP	= true;
}

bool TaskRunner::isDebugMode(void)
{
	if(m_exeMode == DEBUG)
		return true;
	return false;
}

bool TaskRunner::isMonitorMode(void)
{
	if(m_exeMode == MONITOR)
		return true;
	return false;
}
/*

int TaskRunner::dbgVariables(ExeTaskTreeNode* node, RFSMvariable* var, int line)
{
	//�̸��� 0.1.2.abc �ε� ���� �̸��� ���ٸ�
	//0.1.abc, 0.abc� �˻��ؾ� �Ѵ�.

	ValueMap* varTAB = node->getVariableTAB();

	std::string blockpath = var->getBlockPath();
	std::vector<std::string> path = Estring::split(blockpath, ".");

	for(unsigned int k=path.size(); k>0; k--){
		std::string c_path="";
		for(unsigned int i=0; i<k; i++){
			c_path.append(path.at(i));
			if(i+1<k)c_path.append(".");
		}

		ValueMap::iterator it;
		for(it = varTAB->begin(); it!=varTAB->end();it++){
			std::string	vname = it->first;
			RFSMvalueEval value = it->second;
		}


//		if(m_localVarList->moveHead()){
//			do{
//				TABsymbol *sym = m_localVarList->getCurObject();
//				if(sym->isSameName(name) && sym->getBlockPath().compare(c_path)==0)
//					return 0;
//			}
//			while(m_localVarList->moveNext());
//		}
		
	}
	
	return SYM_LOCAL_VAR_NOT_FOUND;
}

*/
//Model�� ���ǵ� fuction/action -------------------------------------------------------------------------------
RFSMvalueEval TaskRunner::bind4Function(ExeTaskTreeNode* node, unsigned int callType, RFSMcallFunc* rfunc, ValueMap* params)
{
	//std::string bname = rfunc->getBlockFullName();
	//std::string key = rfunc->getFileName();
	std::string bname = ((RFSMstmt*)rfunc)->getBlockName();
	std::string name = rfunc->getName();
	int line = ((RFSMvalue*)rfunc)->getEndLine(); //RFSMstmt*�� RFSMvalue* �ΰ��߿� �ϳ��� casting
	int type = rfunc->getType();

	if(DbgTable::s_hasEmulTarget && DbgTable::isEmulTarget(name)){
		return getEmulvalue(name, type, line, bname);
	}
	else{
		RFSMvalueEval rval = m_binder->callFunction(m_logFile, callType, bname, line, type, name, params);
		if(m_exeMode == DEBUG || m_exeMode == MONITOR){
			m_modelTAB.insert(ValuePair(name, rval));
		}

		return rval;
	}
}


//Model�� ���ǵ� symbol -------------------------------------------------------------------------------
RFSMvalueEval TaskRunner::bind4GetModelSym(ExeTaskTreeNode* node, RFSMvariable* sym, int line)
{
	std::string bname = sym->getBlockName();
	std::string name = sym->getName();
	int type = sym->getType();
	if(DbgTable::s_hasEmulTarget && DbgTable::isEmulTarget(name)){
		return getEmulvalue(name, type, line, bname);
	}
	else{
		RFSMvalueEval val = m_binder->getModelValue(m_logFile, sym, line);	
		if(m_exeMode == DEBUG || m_exeMode == MONITOR){
			m_modelTAB.insert(ValuePair(name, val));
		}
		return val;
	}
}

void TaskRunner::bind4SetModelSym(ExeTaskTreeNode* node, RFSMvariable* lvar, RFSMvalueEval symVal, int line)
{	
	std::string name = lvar->getName();
	std::string bname = lvar->getBlockName();
	int type = symVal.getType();
	if(DbgTable::s_hasEmulTarget && DbgTable::isEmulTarget(name)){
		EmulInfo einfo = DbgTable::getEmulInfo(name);
	
		if(type == TYP_INT){
			einfo.value = int2str(symVal.getIntValue());
		}
		if(type == TYP_FLOAT){
			einfo.value = float2str(symVal.getFloatValue());
		}
		if(type == TYP_STRING){
			einfo.value = symVal.getStrValue();
		}
		DbgTable::updateEmulInfo(name, einfo);
	}
	else{
		m_binder->setModelValue(m_logFile, lvar->getName(), symVal);
		if(m_exeMode == DEBUG || m_exeMode == MONITOR){
			m_modelTAB.insert(ValuePair(name, symVal));
		}
	}
}

//Model�� ���ǵ� gvar -------------------------------------------------------------------------------
RFSMvalueEval TaskRunner::bind4GetGlobalVar(ExeTaskTreeNode* node, RFSMvariable* var, int line)
{	
	std::string name = var->getName();
	RFSMvalueEval val = m_binder->getGlobalValue(m_logFile, name);
	if(m_exeMode == DEBUG || m_exeMode == MONITOR){
		m_modelTAB.insert(ValuePair(name, val));
	}
	return val;
}

void TaskRunner::bind4SetGlobalVar(ExeTaskTreeNode* node, RFSMvariable* lvar, RFSMvalueEval val, int line)
{	
	std::string name = lvar->getName();
	m_binder->setGlobalValue(m_logFile, name, val);
	if(m_exeMode == DEBUG || m_exeMode == MONITOR){
		m_modelTAB.insert(ValuePair(name, val));
	}
}

//Model�� ���ǵ� wvar (ExeTask�� ���ǵǾ� �ִ�)-------------------------------------------------------
void TaskRunner::bind4SetTaskVar(ExeTaskTreeNode* node, RFSMvariable* lvar, RFSMvalueEval val, int line)
{
	std::string name = lvar->getName();
	m_eTask->setWorkerVar(name, val);
	if(m_exeMode == DEBUG || m_exeMode == MONITOR){
		m_modelTAB.insert(ValuePair(name, val));
	}
}

RFSMvalueEval TaskRunner::bind4GetTaskVar(ExeTaskTreeNode* node, RFSMvariable* lvar, int line)
{
	std::string name = lvar->getName();
	RFSMvalueEval val = m_eTask->getWorkerVar(name);	
	if(m_exeMode == DEBUG || m_exeMode == MONITOR){
			m_modelTAB.insert(ValuePair(name, val));
	}
	return val;
}

//Behavior�� ���ǵ�Local ���� (ExeBehavior�� ���ǵǾ� �ִ�)-----------------------------------------
void TaskRunner::bind4SetLocalVar(ExeTaskTreeNode* ebhv, RFSMvariable* lvar, RFSMvalueEval val, int line)
{
	//std::string bid = lvar->getBlockPath();
	std::string varName = lvar->getBlockID();
	varName.append(".").append(lvar->getName());
	//���ú����� ���ι�ȣ�� val��, r-value�� �ƴ� lvar�� �������� �����ؾ��Ѵ�.
	int lvarline = lvar->getEndLine();
	val.setLine(lvarline);
	ebhv->addLocalValue(varName, val);
}

RFSMvalueEval TaskRunner::bind4GetLocalVar(ExeTaskTreeNode* node, RFSMvariable* var, int line)
{
	//���ú����� blockpath�� �ٿ��� �˻��ؾ��Ѵ�.
	std::string name = var->getBlockID();
	name.append(".").append(var->getName());
	RFSMvalueEval val = node->getLocalValue(name);
	return val;
}

RFSMvalueEval TaskRunner::getEmulvalue(std::string targetname, int type, int line, std::string bname)
{
	RFSMvalueEval vEval(line, bname);

	EmulInfo einfo = DbgTable::getEmulInfo(targetname);
	
	if(einfo.etype == E_value){		
		if(type == TYP_INT){
			if(einfo.value !="") vEval.setIntValue(str2int(einfo.value));
		}
		if(type == TYP_FLOAT){
			if(einfo.value !="") vEval.setFloatValue(str2float(einfo.value));
		}
		if(type == TYP_STRING){
			vEval.setStrValue(einfo.value);
		}
		if(type == TYP_VOID){
		}
	}
	if(einfo.etype == E_randomF_G){
		vEval.setIntValue(gaussianRandomF(einfo.min, einfo.max));
	}
	if(einfo.etype == E_randomF_U){
		vEval.setIntValue(randomF(einfo.min, einfo.max));
	}
	if(einfo.etype == E_randomD_G){
		vEval.setIntValue(gaussianRandomD(einfo.min, einfo.max));
	}
	if(einfo.etype == E_randomD_U){
		vEval.setIntValue(randomD(einfo.min, einfo.max));
	}
	if(einfo.etype == E_enum){
		int idx = randomE(einfo.enumlist.size());
		if(idx>=0){ 
			std::string valstr =  einfo.enumlist[idx];
					
			if(type == TYP_INT){
				if(valstr !="") vEval.setIntValue(str2int(valstr));
			}
			if(type == TYP_FLOAT){
				if(valstr !="") vEval.setFloatValue(str2float(valstr));
			}
			if(type == TYP_STRING){
				vEval.setStrValue(valstr);
			}
			if(type == TYP_VOID){
			}
		}
	}

	return vEval;
}

//------------���� �Լ�----------------------------------------------------------------------------
/*
void TaskRunner::monitorVariable(unsigned int type, RFSMvariable* lvar, RFSMvalueEval val)
{	
	monitorVariable(type, lvar->getName(), val);
}
*/

/*
void TaskRunner::monitorVariable(std::string file, int line, std::string blockname, int threadID, unsigned int type, std::string name, RFSMvalueEval val)
{	
	MonitorResponse* monRes = m_eTask->getMonitorRes();
	monRes->init(type);
	monRes->addVariable(name, val);

	std::string monMsg = monRes->makeResponseMsg(m_eTask->getTask()->getName(), threadID, file, line, m_eTask->getCurTarget(), val.getLine());
	//printf("%s\n", monMsg.c_str());
	m_eTask->sendMonitorMsg(monMsg);
}
*/

/*
void TaskRunner::monitorFunction(std::string file, int line, std::string blockname, int threadID, std::string fname, ValueMap* params)
{
	MonitorResponse* monRes = m_eTask->getMonitorRes();
	monRes->init(RES_DATA_func);
	monRes->addFunc(fname);

	if(params != NULL){
		ValueMap::iterator it;
		for(it=params->begin() ; it!=params->end() ; it++){
			std::string pName = it->first;
			RFSMvalueEval val = it->second;
			//monRes->init(RES_DATA_param);
			monRes->addFuncParam(pName, val);
		}
	}

	std::string monMsg = monRes->makeResponseMsg(m_eTask->getTask()->getName(), threadID, file, line, m_eTask->getCurTarget(), line);
	//printf("%s\n", monMsg.c_str());
	m_eTask->sendMonitorMsg(monMsg);
}
*/


/*
void TaskRunner::debug_print(std::string msg)
{
	pthread_mutex_lock(&ml);
	printf("%s\n", msg.c_str());
	pthread_mutex_unlock(&ml);
}
*/