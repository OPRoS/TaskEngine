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
	//m_logFile은 TaskRunner를 사용하는 외부 즉, bhv에서 사용하는 거랑 같은 걸 사용하기 때문에
	//여기서 삭제하지 않는다.
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
		//construct 실행
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

	//자식이 있다면
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

	//1.<Construct 실행>
	if(!exeTask->isConstructed()){
		exeTask->setConstructed();
		//1.1 Var
		std::vector<RFSMstmt*>* varAsmts = rfsmtask->getVarAsmts();
		if(varAsmts != NULL){//varAsmts내에는 RFSMasmt와 RFSMvar 두개만 존재			
			FLOG_TRACE(m_logFile, " [VAR]");	
			exeTask->setCurTarget(exeTask->getNodeName() + ".var");
			enterActionBlock(varAsmts, exeTask);
		}
		//1.2 construct 블록 실행
		enterConstruct(exeTask);
		//return;
	}

	//2.<expand할 behavior|conexer 선택>
	std::vector<ExeTransition*> translist;
	std::vector<RFSMstmt*>* runStmts = rfsmtask->getRunBlock();
	RFSMStmtBlock* enclosing = NULL;
	if(runStmts != NULL && runStmts->size() > 0){
		FLOG_TRACE(m_logFile, " [RUN]");
		exeTask->setCurTarget(exeTask->getNodeName() + ".run"); 
		enterDecisionBlock(runStmts, exeTask, translist, false);
	}

	//3.<expand 실행>
	//3.1 이전 상태와 현재 조건 비교, target이 nothing일 때도 not valid는 지워야 한다.
	removeNotValidSubs(exeTask, translist);	
	
	//3.2 비교 결과 적용하여 실행
	link4translist(exeTask, translist);

	//3.3 선택된 하위 노드의 완료여부 검사
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
	//3.4 translist 삭제
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

		//일단 sub노드를 상태를 검사
		for(unsigned int i=0; i<cNodes->size();i++){
			updateConnectorStatus(subExeCon, cNodes->at(i));
		}

		int conType = subExeCon->getConType();
		int joinType = subExeCon->getJoinType();				
		bool iscompleted = false;
		if(subExeCon->isRunnedAfterConstBL() && cNodes->size()<1){
			//connector에 하위 노드가 없으면
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
				//완료되면
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
					subExeCon->stopRunning();//전부 종료						
					enterChildNodeDestruct(subExeCon);
					subExeCon->removeAllChildNodes(false); //isTaskEnd=false
					break;
				}
			}
			if(iscompleted){
				//완료되면
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
				//전체 완료
				if(subExeCon->getCurActiveSeq() == subExeCon->getNumOfRunBlocks()){
					subExeCon->setCompleted(true);
printf("%s: an seqexer is completed\n", subExeCon->getNodeName().c_str());
				}
				//중간노드 완료
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

	//1.<Construct 실행>
	if(!exeTask->isConstructed()){
		exeTask->setConstructed();
		//1.1 Var
		std::vector<RFSMstmt*>* varAsmts = rfsmtask->getVarAsmts();
		if(varAsmts != NULL){//varAsmts내에는 RFSMasmt와 RFSMvar 두개만 존재			
			FLOG_TRACE(m_logFile, " [VAR]");	
			exeTask->setCurTarget(exeTask->getNodeName() + ".var");
			enterActionBlock(varAsmts, exeTask);
		}
		//1.2 construct 블록 실행
		enterConstruct(exeTask);
		//return;

		//2.<expand할 behavior|conexer 선택>
		std::vector<ExeTransition*> translist;
		std::vector<RFSMstmt*>* runStmts = rfsmtask->getRunBlock();
		RFSMStmtBlock* enclosing = NULL;
		if(runStmts != NULL && runStmts->size() > 0){
			FLOG_TRACE(m_logFile, " [RUN]");
			exeTask->setCurTarget(exeTask->getNodeName() + ".run"); 
			enterDecisionBlock(runStmts, exeTask, translist, false);
		}

		//3.<expand 실행>
		//3.1 이전 상태와 현재 조건 비교, target이 nothing일 때도 not valid는 지워야 한다.
		removeNotValidSubs(exeTask, translist);	
		
		//3.2 비교 결과 적용하여 실행
		link_Expand(exeTask, translist);

		//3.3 선택된 하위 노드의 완료여부 검사
		/* //한번 선택된 sub노드를 바꿀때 (예전버전)
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
		//3.4 translist 삭제
		std::vector<ExeTransition*>::iterator it;
		for(it=translist.begin() ; it!=translist.end() ; ++it){
			delete (*it);
		}
		translist.clear();

	}
	//하위노드가 생성된 이후
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

	//1. var 실행
	if(params.size()>0){
		m_eTask->setCurTarget(exeCon->getNodeName() + ".param"); 
		const std::string taskBlockID = exeCon->getBlockID();
		ValueMap::iterator it;
		for(it=params.begin(); it!=params.end() ;it++){
			//호출한 위치의 값(RFSMValueEval)이 그대로 전달 되었기 때문에, 파라메터의 위치와 target으로 바꿔야 한다. 
			it->second.setLine(exeCon->getNodeNameLine()); 
			it->second.setTarget(m_eTask->getCurTarget());
			//이름에 태스크 블럭을 붙여야 한다.
			std::string localVarName = taskBlockID;
			localVarName.append(".").append(it->first);			
			exeCon->addLocalValue(localVarName, it->second); //behavior 파라메터는 그냥 라인을 0으로 설정
		}
		params.clear();
		//delete params;//나중에 auto_ptr로 바꾸자...
	}

	//2. construct 실행
	RFSMconnector* rfsmCon = m_eTask->getConnector(exeCon->getNodeName());	
	std::vector<RFSMstmt*>* varAsmts = rfsmCon->getVarAsmts();
	if(!exeCon->isConstructed()){
		exeCon->nodeStart();//BEHAVIOR_TIME시작
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

	//3. 동실실행 하위 노드(with문 추출)
	exeCon->enterAfterConstBL(true);
	std::vector<ExeTransition*> translist;
	RFSMStmtBlock* withsblock = rfsmCon->getRunBlocks();
	if(withsblock != NULL){
		std::vector<RFSMstmt*>*	withstmts = withsblock->getStmtBlock();
		enterDecisionBlock(withstmts, exeCon, translist, true);
	}
	exeCon->setWithList(translist);

	//4 translist에 포함된 각각의 transition처리
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
			//호출한 위치의 값(RFSMValueEval)이 그대로 전달 되었기 때문에, 파라메터의 위치와 target으로 바꿔야 한다. 
			it->second.setLine(exeBhv->getNodeNameLine()); 
			it->second.setTarget(m_eTask->getCurTarget());
			//이름에 태스크 블럭을 붙여야 한다.
			std::string localVarName = taskBlockID;
			localVarName.append(".").append(it->first);			
			exeBhv->addLocalValue(localVarName, it->second); //behavior 파라메터는 그냥 라인을 0으로 설정
		}
		params.clear();
		//delete params;//나중에 auto_ptr로 바꾸자...
	}
	
	//1. Behavior 실행인스턴스의 이름으로 부터 RFSMbehavior(구조)를 얻어온다.
	RFSMbehavior* bhv = m_eTask->getBehavior(exeBhv->getNodeName());	

	//2. construct 실행
	if(!exeBhv->isConstructed()){
		std::vector<RFSMstmt*>* varAsmts = bhv->getVarAsmts();
		exeBhv->nodeStart();//BEHAVIOR_TIME시작
		exeBhv->setConstructed();
		//2.1 Behvaior Var
		if(varAsmts != NULL){			
			FLOG_TRACE(m_logFile, " [VAR]");	
			m_eTask->setCurTarget(exeBhv->getNodeName() + ".var");
			enterActionBlock(varAsmts, exeBhv);
		}
		//2.2 Construct 블럭을 실행하면 EOI(return)
		//if(enterConstruct(exeBhv))return;
		enterConstruct(exeBhv);
	}

	//3. Behavior의 실행인스턴스로 부터 현재 상태를 파악
	std::string curSName = exeBhv->getCurState(); //Task의 현재 State를 가져옴

	//4. 현재 상태의 구조를 얻는다.
	RFSMstate* state = bhv->getState(curSName);

	//5. 상태를 실행
	if(state != NULL) enterState(exeBhv, bhv, state);

}

/*
[실행시 제어 순서]
시작
  Task생성 →초기state →(Entry) → (Do)→한번의 사이클 끝
재입력
  경우1:(Transition:stay)→(Do)→ 한번의 사이클 끝
  경우2:(Transition:stay)→(Do)→(Exit)→ Subtask생성 →(Entry) →(Do) →한번의 사이클 끝
  경우3:(Transition:state)→(Exit)→다른State→(Entry) →(Do) →한번의 사이클 끝

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
		//Entry 실행
		if(entrystmts != NULL){
			m_eTask->setCurTarget(stName + ".entry"); //stname에 behavior이름이 포함
			//updateMonitoringStatus();
			FLOG_TRACE(m_logFile, " (ENTRY)--------------");
			ebhv->setCurStateBlock(ENTRY_BLOCK);
			//actionPerformed = enterActionBlock(entrystmts, ebhv);
			//if(actionPerformed) return;
			//entry가 비어 있어도 그냥 iteration이 끝나는게 맞음
			enterActionBlock(entrystmts, ebhv);
		}
		return;
	}

	//2.Decision:어디로 천이될지 결정
	std::vector<ExeTransition*> translist;
	std::string nextTarget = ebhv->getNextTarget();
	if(nextTarget != ""){
		//2.1 만약 현재 실행 behavior의 하위 behavior가 완료되고 ~>에 의해 천이되는 경우
		ExeTransition* t = new ExeTransition(TO_STATE, nextTarget, "", -1, -1);
		ebhv->setNextTarget("");
		translist.push_back(t);
	}
	else{
		std::vector<RFSMstmt*>* trans = state->getTransition();
		if(trans!= NULL && trans->size()>0){
			m_eTask->setCurTarget(stName + ".trans");//stname에 behavior이름이 포함
			//updateMonitoringStatus();
			FLOG_TRACE(m_logFile, " (TRANSITION)---------");
			ebhv->setCurStateBlock(TRANS_BLOCK);
			//moveto = enterDecisionBlock(trans, ebhv);
			enterDecisionBlock(trans, ebhv, translist, false);
		}
	}

	//transition block내에서 디버깅하다가 강제 종료하면
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
//에러 출력 bebavior와 state를 동시에 전이 할 수 없다.
//다수의 behavior만 병렬로 동시 전이가 가능하다.
		}
		
		if(transNum == 1 && isState){
			moveto = translist[0];
		}
	}

	//남아 있는경우, transition 조건에서 만족되는 게 아무것도 없다.
	if(translist.size() < 1){
		//아무것도 안한다.
		//delete moveto;

		return;
	}
	//천이되는 경우
	else{
		//3. decision 결과 획득			
		if(transNum == 1 && isState){ //3.1 moveto인 경우
			FLOG_TRACE(m_logFile, "   (RUNNING:Goto) %s ", moveto->getName().c_str());
			std::string target = moveto->getName();

//this인경우 isBehavior가 false인지 확인요망
			
			//4.Do	
			if(target.compare("stay")==0){
				//현재 상태에 머무른다.
				std::vector<RFSMstmt*>* dostmts = state->getDostmts();
				if(dostmts != NULL){
					m_eTask->setCurTarget(stName + ".stay"); //stname에 behavior이름이 포함
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
					전이 발생
				************************/			
				//5.Exit
				if(transNum == 1 && isState){										
					std::vector<RFSMstmt*>* exitstmts = state->getDestruct();
					if(exitstmts != NULL){
						//exit문에 stmt가 존재하는 경우
						m_eTask->setCurTarget(stName + ".exit"); //stname에 behavior이름이 포함
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
					//exit가 있던 없든 exit를 실행했다고 간주
					//exit를 수행하고 무조건 전이가 발생한다.시작 시점에서 actionPerfromed를 점검
					ebhv->exitExecuted();
		
					//ebhv->setBehaviorInCurState(NULL); //현재 상태의 subbehavior들을 delete하기 위해
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
		else if(isBhv || isCon){//3.2 expand인 경우
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

			//RFSMblockStmt에 대한 검사는 반드시 isBreakline보다 먼저 와야 한다. 
			//블록의 끝 '}'의 위치가 RFSMblockStmt의 EOL이기 때문에 블록내부 stmt가 다 실행되고 블록 끝이 브레이크 되어야 한다.
			// transition과 run은 RFSMblockStmt가 root이다.
			if(stmt->getClassName()==CLS_RFSMblockStmt){
				RFSMStmtBlock* bstmt = (RFSMStmtBlock*)stmt;
				enterDecisionBlock(bstmt->getStmtBlock(), etask, translist, false);
			}

			else //이부분이 활성화 되면 닫히는 괄호는 무시하게 된다. 
				isOnBreakStmt = onBreak(etask, stmt, stmt->getBlockID());
			//if(isBreakline(etask, stmt->getFileName(), stmt->getEOL())) {
				//1. 변수 태이블 검색후 관련 정보 전송. 변수 테이블에 blockpath나 아이디..이용
			//	etask->sendDebugMsg(m_eTask->getTask()->getName(), stmt->getEOL(), stmt->getBlockPath());
				//2. 대기 
			//	if(m_exeMode == DEBUG) waitAtBreakline(etask, stmt);
			//}

			//if(typeid(*stmt)==typeid(RFSMifStmt) ){
			if(stmt->getClassName().compare(CLS_RFSMifStmt)==0){
				RFSMifStmt* ifstmt = (RFSMifStmt*)stmt;
				//transition = enterDecisionStmt(ifstmt, etask);
				//조건 평가 -> true, false
				RFSMboolable* cond = ifstmt->getCondition();
				bool result = evalCondition(cond, etask, ifstmt->getEndLine());

				//평가에 해당되는 statements
				std::vector<RFSMstmt*>* bStmts = NULL;
				if(result) bStmts = ifstmt->getTrueStmts();
				else bStmts = ifstmt->getFalseStmts();

				unsigned int beforeSize = translist.size();
				enterDecisionBlock(bStmts, etask, translist, isPar);
				if(translist.size()>beforeSize){
					//매우 중요함 전이가 발생하면 (구조적으로 다음 if문이 내포됨) 다른 if문을 더이상 보지 말아야 한다.
					break;
				}
			}
			else if(stmt->getClassName().compare(CLS_RFSMvar)==0){
				//굳이 그냥 변수명이라 할일이 없음
			}
			//else if(typeid(*stmt)==typeid(RFSMasmt) ){
			else if(stmt->getClassName().compare(CLS_RFSMasmt)==0){
				//evaluateValue에서 로컬변수에 저장된다.
				RFSMasmt* asmt = (RFSMasmt*)stmt;
				int el = ((RFSMstmt*)asmt)->getEndLine(); //RFSMstmt*와 RFSMvalue* 두개중에 하나로 casting
				evalValue(asmt, etask, el);//에러처리는 evaluateValue내에서...
			}
			//else if(typeid(*stmt)==typeid(RFSMcallFunc) ){
			//그냥 함수 호출이다. 보통 assignment문으로 사용되지만 그냥 함수만쓰는 경우도 있다.
			else if(stmt->getClassName().compare(CLS_RFSMcallFunc)==0){

				ValueMap* paramsEval = NULL;
				RFSMcallFunc* rfunc = (RFSMcallFunc*)stmt;
				std::vector<RFSMparam*>* params = rfunc->getParameters();
				if(params != NULL){
					int el = ((RFSMvalue*)rfunc)->getEndLine(); //RFSMstmt*와 RFSMvalue* 두개중에 하나로 casting
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
			//subBehavior를 호출하는 경우
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
			//다른 state로 전이되는 경우
			else if(stmt->getClassName().compare(CLS_RFSMgotoStmt)==0){
				RFSMgotoStmt* gotostmt = (RFSMgotoStmt*)stmt;
				ExeTransition* t = new ExeTransition(TO_STATE, gotostmt->getName(), "", stmt->getEndLine(), -1);
				translist.push_back(t);
				if(!isPar) return;				
			}
			else if(stmt->getClassName().compare(CLS_RFSMsymbStmt)==0){
				//그냥 변수명만 a;
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
				//나머지는 error
			}
			//stay이 문에 대해서 .....
		}
	}	
}

void TaskRunner::removeNotValidSubs(ExeTaskTreeNode* curNode, std::vector<ExeTransition*> translist)
{
	//1. 새로운 전이에 포함안되는 sub node를 삭제
	std::vector<ExeTaskTreeNode*> blist = curNode->getAllChildNodes();
	if(blist.size()>0){
		for(unsigned int j=0; j<blist.size(); j++){//현재 포함된 sub behavior
			//ExeBehavior* pBhv = (ExeBehavior*)blist[j];
			std::string nodeName = blist[j]->getNodeName();	
			//std::string nodeId	 = blist[j]->getBlockID();
			int callLine = blist[j]->getCallStmtLine();
			bool vaild = false;
			
			//translist가 0인경우 vaild가 false이므로 blist[j]가 지워진다.

			for(unsigned int i=0; i<translist.size();i++){				
				//타겟의 블록과 이름을 획득
				ExeTransition* et = translist[i];
				std::string targetName = et->getName();
				std::string targetId ="";
				if(et->getTransType()==TO_BEHAVIOR){
					RFSMbehavior* tBhv = m_eTask->getBehavior(targetName);
					targetId = tBhv->getBlockID();//block은 다른 block인데 전이하는 behavior가 같은 경우도 있다. (다시 시작해야함)
				}
					
				if(et->getTransType()==TO_CONNECTOR){
					RFSMconnector* conn = m_eTask->getConnector(targetName);
					targetId = conn->getBlockID();
				}
				
				//타겟과 현재 존재 behavior비교
				//if(nodeName.compare(targetName)==0 && nodeId.compare(targetId)==0){
				if(nodeName.compare(targetName)==0 && callLine==translist[i]->getStmtLine()){
					//기존 것이 (현재에도)여전히 유효
					vaild = true;
				}					
			}
						
			if(!vaild){	//기존 대상이 새로운 대상에 없으면 기존 대상 삭제
				//curEBhv->removeSubBhv(pBhv->getBehaviorName());
				if(blist[j]->isMission() && !blist[j]->isCompleted()){}//미션 수행 중이면 destruct하지 않는다.
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
Behavior 1개 이상 또는
Behavior + Connector 1개 이상 또는
Connector 1개이상으로 구성
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
		//현재 노드가 ExeBehavior이면 translist는 한개이다. 
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
				//만약 전이가 state이면 이건 에러.(원래는 파싱에서 에러를 찾아야 한다.)
			}
		}
	}

	/*
	//Behavior의 경우 ~> 연산자를 사용할 수 있기 때문에.. 
	if(typeid(*curNode)==typeid(ExeBehavior)){
		std::string target = translist[0]->getName();
		int line = translist[0]->getStmtLine();
		ExeTaskTreeNode* childnode = curNode->getChildNode(target, line);
		if(childnode != NULL && childnode->isCompleted()){
			enterDestruct(childnode);
			curNode->removeChildNode(target, false, line); //완료된 노드를 삭제
			std::string nextTarget = translist[0]->getNextTargetState();
			if(nextTarget != ""){//완료후 천이가 존재하면 이동
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
			curNode->removeChildNode(target, false, line); //완료된 노드를 삭제
			std::string nextTarget = translist[0]->getNextTargetState();
			if(nextTarget != ""){//완료후 천이가 존재하면 이동
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
여기서는 curNode가 아니라 linkto가 connector라는 뜻이다. 
*/
void TaskRunner::expand_Connector(ExeTaskTreeNode* curNode, ExeTransition* linkto)
{
	int			calledLine	= linkto->getStmtLine();
	std::string target		= linkto->getName();
	ValueMap*	pas			= linkto->getParameters();
	ValueMap	params;

	if(pas != NULL) params = *pas;

	//case 1: 재진입
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
			//여기서 각각의 runblock에 대해서 decisionBlock의 결과로 translist를 얻어와서 실행함
			enterConnector(subExeCon, params);
			
			//1.1 connector(linkto)에 연결된 모든 노드 획득
			int conType = subExeCon->getConType();
			int joinType = subExeCon->getJoinType();		
			std::vector<ExeTaskTreeNode*> cNodes = subExeCon->getAllChildNodes();		
			bool iscompleted = false;
			if(subExeCon->isRunnedAfterConstBL() && cNodes.size()<1){
				//connector에 하위 노드가 없으면
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
					//완료되면
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
						subExeCon->stopRunning();//전부 종료						
						enterChildNodeDestruct(subExeCon);
						subExeCon->removeAllChildNodes(false); //isTaskEnd=false
						break;
					}
				}
				if(iscompleted){
					//완료되면
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
					//전체 완료
					if(subExeCon->getCurActiveSeq() == subExeCon->getNumOfRunBlocks()){
						subExeCon->setCompleted(true);
printf("%s: an seqexer is completed\n", subExeCon->getNodeName().c_str());
					}
					//중간노드 완료
					else{
						subExeCon->setCurActiveSeq(seq+1);
					}
				}
			}

			if(iscompleted)
				onBreak(curNode, withsblock, withsblock->getBlockID());
			//if(iscompleted && isBreakline(curNode, withsblock->getFileName(), withsblock->getEOL())) {
				//1. 변수 태이블 검색후 관련 정보 전송. 변수 테이블에 blockpath나 아이디..이용
			//	curNode->sendDebugMsg(m_eTask->getTask()->getName(), withsblock->getEOL(), withsblock->getBlockPath());
				//2. 대기 
			//	if(m_exeMode == DEBUG) waitAtBreakline(curNode, withsblock);
			//}
*/
			updateConnectorStatus(curNode, subExeCon);

		}//if(exeCon != NULL)	
	}

	//case 2: 처음 진입
	else{
		//2.1 ExeConnector생성
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

		if(newConn == NULL){ //else를 하면 안된다.
			int prd = linkto->getPeriod();
			if(prd < 0) prd = 10;
			newConn = new ExeConnector(m_eTask->getRemoteCLI(), m_logFile, curNode, target, rfsmCon->getNameLine(), rfsmCon->getBlockID(), rfsmCon->getFileName(), rfsmCon->getSynchIdMap(), calledLine, prd);
			newConn->setConType(conType);
			newConn->setRunType(runType); //connector에 포함되어 있는 behaviore들의 실행 타입
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
	//파라메터 설정
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



	//재진입
	//if(curNode->isChildExist(target, calledline)){//이미 subtask가 생성된 경우
	if(existingSubBhv != NULL){//이미 subtask가 생성된 경우
		
//Thread로 실행 중인데 다시 들어오는 경우(connector의 construct실행 후 리턴 기능 때문에 이 부분 필요)
//if(typeid(*curNode)==typeid(ExeConnector)) return; 

		//ExeBehavior* existingSubBhv = curEBhv->getSubBhvOfCurState(target);

		//ExeBehavior* existingSubBhv = NULL;
		//ExeTaskTreeNode* node = curNode->getChildNode(target, calledline);
		//if(node != NULL) existingSubBhv = (ExeBehavior*)node;

		//1. behavior->behavior
		if(typeid(*curNode)==typeid(ExeBehavior)){//behavior내의 sub behavior는 nothread로 실행
			enterBehavior(existingSubBhv, params);				
		}
		//2. connector->behavior
		else if(typeid(*curNode)==typeid(ExeConnector)){
			//else if(runType == SYNCHRO || runType == ASYNCHRO){//현재 행위가 thread로 실행							
			existingSubBhv->setCurParam(params);			
		}
		//3. task->behavior
		else if(typeid(*curNode)==typeid(ExeTask)){
			enterBehavior(existingSubBhv, params);		
		}

		//goal state에 도달 여부 체크
		if(existingSubBhv->isOnGoalState() && existingSubBhv->isEntryExecuted()) 
			existingSubBhv->setCompleted(true);
		
	}
	//새로운 진입
	else{
		//전이될 bhv획득
		RFSMbehavior* newSubBhv = m_eTask->getBehavior(target);
		//initial		
		std::string isname = newSubBhv->getInitialStateName();
		//goal
		std::vector<std::string> gsnames = newSubBhv->getGoalStateNames();
		//run 타입
		int r = runType;
		if(typeid(*curNode)==typeid(ExeBehavior)) r = NOTHREAD;
	
		ExeBehavior* exeBhv = NULL;
		bool isMission = newSubBhv->isMission();
		bool reload = false;
		std::string key = target + int2str(calledline);
		
		if(isMission){
			exeBhv = (ExeBehavior*)(m_eTask->getMissionNode(key));
		}

		if(exeBhv != NULL){ //else를 하면 안된다.
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
		
		//현재 실행 bhv의 자식 bhv로 생성된 실행 bhv 지정
		curNode->addChild(exeBhv); 

		//태스크 실행
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
			//부모노드가 Behavior이면 RUN_NO_THREAD로 실행됨
			if(m_traceThreadON)
				LOG_TRACE(m_log, " (TaskRunner:ExeTaskTreeNode<%s>, enclosed in <%s>) is created", exeBhv->getNodeName().c_str(), curNode->getNodeName().c_str());

			enterBehavior(exeBhv, params);			
		}
		//delete exeTask;
	}
}


/**
Action Block에 포함된 If문
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

			//RFSMblockStmt에 대한 검사는 반드시 isBreakline보다 먼저 와야 한다. 
			//블록의 끝 '}'의 위치가 RFSMblockStmt의 EOL이기 때문에 블록내부 stmt가 다 실행되고 블록 끝이 브레이크 되어야 한다.
			if(stmt->getClassName()==CLS_RFSMblockStmt){
				RFSMStmtBlock* bstmt = (RFSMStmtBlock*)stmt;
				enterActionBlock(bstmt->getStmtBlock() , ebhv);
				stmt->setPerformed();
			}

			onBreak(ebhv, stmt, stmt->getBlockID());
			//if(isBreakline(ebhv, stmt->getFileName(), stmt->getEOL())) {
				//std::string temp = int2str(stmt->getEOL());
				//1. 변수 테이블 검색후 관련 정보 전송. 변수 테이블에 blockpath나 아이디..이용
				//ebhv->sendDebugMsg(m_eTask->getTask()->getName(), stmt->getEOL(), stmt->getBlockPath());
				//2. 대기 
				//if(m_exeMode == DEBUG) waitAtBreakline(ebhv, stmt);
			//}

			if(stmt->getClassName().compare(CLS_RFSMsysSynch)==0){
				RFSMsysSynch* rfsmsyn = (RFSMsysSynch*)stmt;
				RFSMvalue* t = rfsmsyn->getValue();
				RFSMvalueEval eval = evalValue(t, ebhv, rfsmsyn->getEndLine());	
/*		
				//----------------Begin(synch)------------------//
				int synchid = eval.getIntValue();
				while(m_eTask->isLocked_Synch(synchid) && !m_eTask->isStopped()){	//동시 실행 시작 후, 재 입력될 때
					thread_sleep(1);
				}
				m_eTask->waitingCntPlus(synchid);
				ebhv->notifyWaitingID(synchid);			//1. SYNCH문을 만날 때 마다 부모노드에게 알림
				m_eTask->updateWaitingIDs();			//2. 최상위 노드에서 아직 실행하면 안되는 synch id를 찾기
	bool p = true;
				while(m_eTask->isWaitingID(synchid) && !m_eTask->isStopped()){//3. 모두 synch될 동안 대기		
	if(p){
		printf("%s: SYNCH(%d) Waiting\n", ebhv->getNodeName().c_str(), synchid);
		p = false;
	}
					thread_sleep(1);
				}
				m_eTask->lock_Synch(synchid);			//4. 동시실행 하기전 해당 synch id를 lock
				m_eTask->waitingCntMinus(synchid);

				if(m_eTask->isAllSynchPassed(synchid)){	//5. 동시실행 되어야 할 모든 id들이 전부 실행 후에	
		printf("All SYNCH(%d) is passed\n", synchid);
					m_eTask->unlock_Synch(synchid);
					m_eTask->removeWaitingID(synchid);
					//m_eTask->resetWaitingCnt();
				}
				//------------------End(synch)------------------//
*/
				//----------------Begin(synch)------------------//
				
				//강제 종료하면 broadcast하는데 그 이후에 wait하면 안된다. 
				if(m_eTask->isStopped()) return false;
				
				int synchid = eval.getIntValue();
				
				m_eTask->waitingCntPlus(synchid);
				//아래 notifyWaitingID가 갱신동안 lock되어야 한다.
				//(다소 지저분해도 이 방법밖에 없음, 함부로 바꾸지말것 치명적 에러)
				pthread_mutex_lock(m_eTask->getMutex());
				//1. 현재 노드의 synch id 정보를 위로 전달
				ebhv->notifyWaitingID(synchid);			 
				//2. synch id에 대해서 계속대기 여부를 알기 위해 root 부터 검사
				//(형제노드의 상황은 모르기 때문)
				m_eTask->updateWaitingID();			
				pthread_mutex_unlock(m_eTask->getMutex());

				pthread_mutex_t* mu	 = m_eTask->getSYNCHMutex(synchid);
				pthread_cond_t*	 co  = m_eTask->getSYNCHCond (synchid);
				pthread_cond_t*  co2 = m_eTask->getSYNCHCond2(synchid);	
//struct timeb current;
//ftime(&current);
//printf("%d : SYNCH-start\n", current);
			
				//pthread_cond_wait,pthread_cond_broadcast등은 항상 pthread_mutex_lock/unlock사이에 사용
				pthread_mutex_lock(mu);
				if(!m_eTask->isWaitingID(synchid)){ 
					//4.synch id 정보 삭제
					m_eTask->removeWaitingID(synchid);
					//5.waiting 전부 해제
					pthread_cond_broadcast(co);
				}
				else if(!m_eTask->isStopped()){
					//3.여기서 synch 되기를 대기, 아래 wait문 내부에서 mu를 다시 unlock한다.
					pthread_cond_wait(co, mu);
				}
				pthread_mutex_unlock(mu);
	
				m_eTask->waitingCntMinus(synchid);
/*
				//synch id 정보 삭제
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
				int el = ((RFSMstmt*)asmt)->getEndLine(); //RFSMstmt*와 RFSMvalue* 두개중에 하나로 casting
				evalValue(asmt, ebhv, el);				
			}
			else if(stmt->getClassName().compare(CLS_RFSMvar)==0){
				RFSMvar* varDecl = (RFSMvar*)stmt;
				int el = ((RFSMvalue*)varDecl)->getEndLine(); //RFSMstmt*와 RFSMvalue* 두개중에 하나로 casting
				evalValue(varDecl, ebhv, el);		
			}
			else if(stmt->getClassName().compare(CLS_RFSMsysWaitfor)==0){
				RFSMsysWaitfor* rfsmwait = (RFSMsysWaitfor*)stmt;
				RFSMvalue* t = rfsmwait->getValue();
				RFSMvalueEval eval = evalValue(t, ebhv, rfsmwait->getEndLine());	
				//thread_sleep(eval.getIntValue());

				// timeout 시간 설정(ex 1510)
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
				//action을 호출하기 이전에 out model값을 반영한다.
				if(params != NULL && paramsEval == NULL){
					FLOG_ERROR(m_logFile, "  (RUNNING:ACTION) %s, Calling action is skipped.", action->toString().c_str());
				}
				else{
					//m_eTask->setOutModelValue();
					m_binder->callFunction(m_logFile, RFSMcall::CALL_ACTION, TYP_VOID, action->getName(), paramsEval);
				}
				////////////////////////////////////////////////////////////////////////////////////////////////
							
				return true; //Action을 호출하면 사이클이 끝. 다음은 의미없는 문..(하나의 action만 실행)
			}
			*/
			//else if(typeid(*stmt)==typeid(RFSMcallFunc) ){
			else if(stmt->getClassName().compare(CLS_RFSMcallFunc)==0){

				ValueMap* paramsEval = NULL;
				RFSMcallFunc* rfunc = (RFSMcallFunc*)stmt;
				std::vector<RFSMparam*>* params = rfunc->getParameters();
				if(params != NULL){
					int el = ((RFSMvalue*)rfunc)->getEndLine(); //RFSMstmt*와 RFSMvalue* 두개중에 하나로 casting
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
				//그냥 변수명만 a;				
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
			else if(stmt->getClassName()==CLS_RFSMwithStmt){//sequential의 경우 실행
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
				//with block내의 stmt들
				std::vector<RFSMstmt*>* stmts = bstmt->getStmtBlock();
				ExeActionBlock* ablock = new ExeActionBlock(m_eTask, 5);
				eablist.push_back(ablock);
				ablock->setParameters(stmts, (ExeBehavior*)ebhv);
				ablock->execute();				
			}
		}
	}	

	//AND-join(현재 다른 것은 고려하지 않고 있다.)
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
			//여기서 파라메터이름에 blockpath를 붙이면 안된다. 함수나 action호출 파라메터도 다루기 때문
			
			paramsEval->insert(ValuePair(param->getName(), pValEval));
		}
	}

	return paramsEval;
}

RFSMvalueEval TaskRunner::evalValue(RFSMvalue* value, ExeTaskTreeNode* ebhv, int line)
{
	//상속관계 때문에 자식이 항상 먼저오고 반드시 else 붙일것

	if(typeid(*value)==typeid(RFSMmulti) ){
		RFSMmulti* bop = (RFSMmulti*)value;
		return evalBinaryOp(bop, ebhv, line);
	}

	if(typeid(*value)==typeid(RFSMadd) ){
		RFSMadd* bop = (RFSMadd*)value;
		return evalBinaryOp(bop, ebhv, line);
	}

	else if(typeid(*value)==typeid(RFSMvar) ){ //변수 선언
		RFSMvar* varDecl = (RFSMvar*)value;
		RFSMvariable* lvar = varDecl->getLvalue();
		//초기화 되지 않았기 때문에 임의의 RFSMvalueEval생성
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

		//테이블에 저장
		//if(lvar->isLocal()){//지역변수인 경우
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
		else if(varMode==G_VAROUT){//model값인 경우 (OUT타입)

			////////////////////////////////////////////////////////////////////////////////////
			//따로 테이블 만들고 action/function 호출하기 전까지 저장한다.
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
		if(params != NULL){//매개변수가 있는경우
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
			//로컬변수는 blockpath를 붙여서 검색해야한다.
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
	//오류 보고
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

		if(op==OP_GRE){//좌변이 기준: 좌변이 크다.
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
		if(op==OP_GRE){//좌변이 기준: 좌변이 크다.
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
		//이건 에러이다.
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
* 중간에 값을 못얻어오면 무조건 false를 리턴한다.
* 그래야.. 죽지 않는다. 값이 제대로 들어오면 동작한다.
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
			eval = true;//시작은 참
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
		//1. 변수 태이블 검색후 관련 정보 전송. 변수 테이블에 blockpath나 아이디..이용
		node->sendDebugMsg(&m_modelTAB, m_eTask->getTask()->getName(), bp->getEndLine(), blockpath);
		//2. 대기 
		if(m_exeMode == DEBUG){ 
			waitAtBreakline(node); //break라인이 해제되기를 기다린다.
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
		
		//초기화
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

		//1.디버깅 대상인지 판단	
		if(toNextBP){			
			isbreakpoint = DbgTable::findByFile(name, line);//정확히 breakline과 일치하는지 검사.
		}
		else if(m_exeMode == DEBUG && toNextStep){//monitor일 때는 다음 라인 이동이 동작안한다.
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

		//2.1 다음에 사용자가 클릭한 명령을 입력받기 위해 리셋을 한다.(밖으로 빼지말 것 위치 중요)
		//m_moveNextStep	= false;
		//m_moveNextBP	= false;

		//2.2 대기
		pthread_mutex_lock(bp_mu);		
		//node->sendDebugMsg(m_eTask->getTask()->getName(), stmt->getEOL(), stmt->getBlockPath());
		pthread_cond_wait(bp_co, bp_mu);
		pthread_mutex_unlock(bp_mu);

		//2.2 대기
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
	//이름이 0.1.2.abc 인데 같은 이름이 없다면
	//0.1.abc, 0.abc등도 검사해야 한다.

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
//Model에 정의된 fuction/action -------------------------------------------------------------------------------
RFSMvalueEval TaskRunner::bind4Function(ExeTaskTreeNode* node, unsigned int callType, RFSMcallFunc* rfunc, ValueMap* params)
{
	//std::string bname = rfunc->getBlockFullName();
	//std::string key = rfunc->getFileName();
	std::string bname = ((RFSMstmt*)rfunc)->getBlockName();
	std::string name = rfunc->getName();
	int line = ((RFSMvalue*)rfunc)->getEndLine(); //RFSMstmt*와 RFSMvalue* 두개중에 하나로 casting
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


//Model에 정의된 symbol -------------------------------------------------------------------------------
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

//Model에 정의된 gvar -------------------------------------------------------------------------------
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

//Model에 정의된 wvar (ExeTask에 정의되어 있다)-------------------------------------------------------
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

//Behavior에 정의된Local 변수 (ExeBehavior에 정의되어 있다)-----------------------------------------
void TaskRunner::bind4SetLocalVar(ExeTaskTreeNode* ebhv, RFSMvariable* lvar, RFSMvalueEval val, int line)
{
	//std::string bid = lvar->getBlockPath();
	std::string varName = lvar->getBlockID();
	varName.append(".").append(lvar->getName());
	//로컬변수의 라인번호는 val즉, r-value가 아닌 lvar의 라인으로 설정해야한다.
	int lvarline = lvar->getEndLine();
	val.setLine(lvarline);
	ebhv->addLocalValue(varName, val);
}

RFSMvalueEval TaskRunner::bind4GetLocalVar(ExeTaskTreeNode* node, RFSMvariable* var, int line)
{
	//로컬변수는 blockpath를 붙여서 검색해야한다.
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

//------------공통 함수----------------------------------------------------------------------------
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