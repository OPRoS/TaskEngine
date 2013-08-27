/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2011.08.16
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright(C) 2011 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "scenario/TPLGenerator.h"

TPLGenerator::TPLGenerator(OSFileSys* fsys, std::string scfname, std::string inclStr, std::vector<STask*> stasks, std::vector<SC*> bslist, std::vector<SBehavior*> sbhvlist, std::vector<SEvent*> sevtlist): CodeWriter(fsys)
{
	m_stasks	= stasks;
	m_sclist	= bslist;
	m_sbhvlist	= sbhvlist;
	m_sevtlist	= sevtlist;
	m_scfname	= scfname;
	m_inclStr	= inclStr; //include리스트

	std::vector<std::string> path_name = Estring::split(scfname, ".");
	m_modelname = path_name[0];
	Estring::replaceAll(m_modelname, "/", "_");

	m_log.setLevel(Log::LOG_LEVEL_ERROR, true);
}

TPLGenerator:: ~TPLGenerator(void)
{
	if(m_bhvTab.size()>0){
		for(m_btIter=m_bhvTab.begin() ; m_btIter!=m_bhvTab.end() ; m_btIter++){
			delete m_btIter->second; 
		}
	}
	m_bhvTab.clear();

	if(m_epdlist.size()>0){
		for(m_epdlistIter=m_epdlist.begin() ; m_epdlistIter!=m_epdlist.end() ; m_epdlistIter++){
			delete m_epdlistIter->second; 
		}
	}
	m_epdlist.clear();

	if(m_fsmTAB.size()>0){
		for(m_fsmTABIter=m_fsmTAB.begin() ; m_fsmTABIter!=m_fsmTAB.end() ; m_fsmTABIter++){
			delete m_fsmTABIter->second; 
		}
	}
	m_fsmTAB.clear();

	//m_stasks, m_sclist,	m_sbhvlist, m_sevtlist들은 parsed result에서 삭제된다.
}

void TPLGenerator::generateTPL(std::string styleFileDir)
{
	//1. 각각의 !scenario블록에 대해서
	for(unsigned int x=0; x<m_sclist.size(); x++){
		//std::string start = "startBhv";
		std::string start = m_sclist[x]->getSCname();
		std::string type = m_sclist[x]->getType();
		std::vector<SBhvSeq*> bslist = m_sclist[x]->getBSlist();
		
		if(bslist.size()<1) return;

		//@goto {begin, end}에 대한 노드 삽입
		for(unsigned int i=0; i<bslist.size(); i++){
			SBhvSeq* bs = bslist[i];
			NodeBS* node = bs->getStartNode();
			setPrefix(start, node);
			bs->makeGotoBhv(start); 
		}
		
		//각각의 !scenario에 대해서 FSM(Behavior) 생성
		FSM4BS* fsm4bs = new FSM4BS(start, "", start, type);
		m_fsmTAB.insert(m_fsmTABPair(start, fsm4bs));

		for(unsigned int i=0; i<bslist.size(); i++){
			SBhvSeq* bs = bslist[i];
			NodeBS* startnode = bs->getStartNode();
			std::vector<std::string> ancestor;		
			detectCycle(startnode, ancestor);
			fsm4bs->addBhvSeq(startnode);
				
			makeFSM(startnode, type);
			//findDuplicatedBhv(node, appearedList, duplicatedList);
		}
	}

	makeFSM4ExptMon();

	//2. FSM에 state를 채워넣기
	for(m_fsmTABIter=m_fsmTAB.begin(); m_fsmTABIter!=m_fsmTAB.end(); m_fsmTABIter++){
		std::string prefix	= m_fsmTABIter->first;
		FSM4BS* fsm			= m_fsmTABIter->second;
		
		std::set<std::string> appearedList;
		std::set<std::string> dupBhvList;	
		std::map<std::string, std::set<std::string>> dup_bhv_prefix_list;

		std::vector<NodeBS*> seqs = fsm->getBhvSeqList();
		for(unsigned int i=0; i<seqs.size(); i++){
			findDuplicatedBhv(seqs[i], appearedList, dup_bhv_prefix_list);
		}
		for(unsigned int i=0; i<seqs.size(); i++){
			addStates4Bhvs(dup_bhv_prefix_list, fsm, NULL, "", seqs[i]);
		}
		
		//addStates4PostCon	(fsm);
		addStates4Branch	(fsm);
		addInitialState		(fsm);
	}



	//3. 파일에 쓰기
	//initStyle("style/scenario.style");
	initStyle(styleFileDir + "/scenario.style");

	std::string tfilename = m_homeDir + "/" + m_scfname;
	Estring::replaceAll(tfilename, ".sc", ""); //sc확장자가 아닌경우 이렇게 안하면 시나리오파일을 덮어씀
	tfilename.append(".tpl");
	FILE* tfile = fopen(tfilename.c_str(),"w");
	if(tfile == NULL){
		//파일 생성오류, 파일명의 오류로 인해서 write할 파일을 생성하지 못함
		LOG_ERROR(m_log, "TPLGenerator can not make the file '%s'.", tfilename.c_str());
		return;
	}

	std::string incblock = getBlock("$include_block");
	Estring::replaceAll(incblock, "%INCLUDE%", m_inclStr);
	fputs(incblock.c_str(), tfile);

	for(unsigned int x=0; x<m_stasks.size(); x++){
		std::string tskblock = getBlock("$task_block");
		Estring::replaceAll(tskblock, "%TNAME%", m_stasks[x]->getName());
		Estring::replaceAll(tskblock, "%START%", m_stasks[x]->getStart());
		fputs(tskblock.c_str(), tfile);
	}

	for(m_fsmTABIter=m_fsmTAB.begin(); m_fsmTABIter!=m_fsmTAB.end(); m_fsmTABIter++){
		std::string prefix = m_fsmTABIter->first;
		FSM4BS* fsm = m_fsmTABIter->second;
		//generateFSMCode(tfile, prefix, fsm);
		generateFSMCode2(tfile, prefix, fsm);
	}
	for(unsigned int i=0;i<m_sbhvlist.size();i++){
		std::string name = m_sbhvlist[i]->getName()->getData();
		std::string pbhvblock = getBlock("$prim_bhv");
		Estring::replaceAll(pbhvblock, "%BHVNAME%", name);
		std::string bhvCode = getBehaviorActions(name);
		Estring::replaceAll(pbhvblock, "%BHV_CODE%", bhvCode);
		fputs(pbhvblock.c_str(), tfile);
	}

	generateModelCode(tfile);
	
	fclose(tfile);
}

void TPLGenerator::generateFSMCode(FILE* tfile, std::string prefix, FSM4BS* fsm)
{
	//1. con
	//if(prefix != "startBhv"){
	if(!isSCname(prefix)){		
		std::string conblock = "";
		if(fsm->getMonOp() == "^" && fsm->isMonitorSelector()){
			//conexer가 필요없다.
		}
		else if(fsm->getMonOp() == "~" && fsm->isMonitorSelector()){
			//conexer
			conblock = getBlock("$con_block4~");
			Estring::replaceAll(conblock, "%ACT_PREFIX_VAR%", attachModelname(prefix));
		}

		Estring::replaceAll(conblock, "%FSMNAME%", fsm->getName());
		Estring::replaceAll(conblock, "%BHVNAME%", fsm->getRefBhv());
		Estring::replaceAll(conblock, "%PREFIX%", prefix);
		Estring::replaceAll(conblock, "%MON_EVT%", prefix);
		fputs(conblock.c_str(), tfile);
	}

	//2. mon
	//$fsm_block@$start_block
	//behavior %BHVNAME%(){
	//%STATES%
	//};
	std::string fsmblock;
	if(fsm->getMonOp() == "^" && fsm->isMonitorSelector()) 
		fsmblock = getBlock("$con_block4^");
	else 
		fsmblock = getBlock("$fsm_block");

	std::vector<NodeState*> states = fsm->getStates();
	std::string stateinMon;
	std::string monVars;
	for(unsigned int i=0; i<states.size();i++){
		std::string stateblock;
		if(fsm->isMonitorSelector() && fsm->getMonOp() == "^")
			//state 블록을 따로 만들지 않고 con_block4^블록의 entry와 transition을 채운다.
			stateblock = getBlock("$state_in_mon_selector^");
		else if(fsm->isMonitorSelector() && fsm->getMonOp() == "~") 
			stateblock = getBlock("$state_in_mon_selector~");
		else // 예외처리를 fsm
			stateblock = getBlock("$state_in_mon");
		std::string exitblock = getBlock("$exit_in_state");
		std::vector<NodeTrans*> transs = states[i]->getTrans();
		std::string stateid = states[i]->getBhvName();
		std::string stateTransStr;
		bool usingExit = false;
		
		
		for(unsigned int j=0; j<transs.size(); j++){ //마지막 state가 아니면 transs가 1보다 크다
			std::string transblock;
			if(fsm->isMonitorSelector()) 
				transblock = getBlock("$trans_in_state2");
			else
				//%IF_EVT% %TRANSTYPE% %TARGET% %NEXT%;
				transblock = getBlock("$trans_in_state");

			unsigned int	ttype		= transs[j]->getTransType();
			std::string		tgname		= transs[j]->getTransTarget();
			std::string		srname		= transs[j]->getTransSource();
			std::string		nxt			= transs[j]->getStateAfterExpand();
			std::string		cond		= transs[j]->getCond();
			std::string		monPrefix	= transs[j]->getMonPrefix();
			
			std::string evtExpr;
			std::string condExpr;
			std::string monCond;
			if(monPrefix != ""){
				//monPrefix는 fsm->isMonitorSelector()을 만족하는 fsm이아니라 selector를 호출하는 state의 transition에 존재함
				condExpr = attachModelname(monPrefix);
				if (cond!= "") condExpr.append(" && ");
			}
			//if(cond == "else") evtExpr = "else";
			if(cond != ""){
				condExpr.append(getEventExpr(cond));
			}
			if(cond != "" || monPrefix != "") evtExpr = "if("+ condExpr + ") ";
			if(j>0) evtExpr = "else " + evtExpr;
			
			Estring::replaceAll(transblock, "%IF_EVT% ", evtExpr);
			std::string ttypeStr = "expand";
			std::string bb = "()";
			if(ttype==2) {
				ttypeStr = "moveto";
				bb="";
			}

			if(fsm->isMonitorSelector()){//state가 하나밖에 없음
				//monitor selector의 entry블록에 들어가 변수 셋팅
				std::string varInSelector = getBlock("$var_in_selector");
				Estring::replaceAll(varInSelector, "%MONPREFIX_VAR%", attachModelname(tgname));
				monVars.append(varInSelector);
			

				//transition 블록의 if문에 들어갈 변수
				Estring::replaceAll(transblock, "%MONPREFIX_VAR%", attachModelname(tgname));
			}
			Estring::replaceAll(transblock, "%TRANSTYPE%", ttypeStr);
			Estring::replaceAll(transblock, "%TARGET%", tgname+bb);
			std::string nxt2;
			if(nxt != "") nxt2 = std::string("~>").append(nxt);
			Estring::replaceAll(transblock, "%NEXT%", nxt2);
			stateTransStr.append(transblock);
		}
	

		//FSM의 마지막 state는 trans가 없다.
		if(transs.size()<1){
			//%IF_EVT% %TRANSTYPE% %TARGET% %NEXT%;
			std::string transblock = getBlock("$trans_in_state");

			Estring::replaceAll(transblock, "%TRANSTYPE%", "expand");
			//std::string tgname = states[i]->getUname();
			std::string prefix = states[i]->getPrefix();
			if(hasNestSeq(prefix))
				//Estring::replaceAll(tgname, "state_", "");
				Estring::replaceAll(transblock, "%TARGET%", stateid + "_CON_" + prefix + "()");
			else
				Estring::replaceAll(transblock, "%TARGET%", stateid + "()");

			Estring::replaceAll(transblock, "%NEXT%", "~> g");

			std::string evtExpr = "";
			if(states[i]->getEvtRef() != "" ) evtExpr = "if("+getEventExpr(states[i]->getEvtRef())+") ";
			Estring::replaceAll(transblock, "%IF_EVT% ", evtExpr);
			
			stateTransStr = transblock;
		}

		//EXIT블록
		/*
		if(prefix != "startBhv" && isBSname(stateid)){
			//behavior sequence를 모두 안쪽으로 집어넣어서 startBhv에서는 exit를 사용하지 않음 
			Estring::replaceAll(exitblock, "%EXIT_STMTS%", stateid + " = true;");
			usingExit = true;
		}
		else if(prefix == "startBhv" && stateid == "SC"){
			std::string exits = "SC = false;";
			Estring::replaceAll(exitblock, "%EXIT_STMTS%", exits);
			usingExit = true;
		}
*/
		//%INI% state %SNAME%{
		//	transition{
		//		%TRANSS%
		//	}
		//}
		std::string sname = states[i]->getState();
		std::string ini;
		if(states[i]->isInitial()) ini = "initial "; //뒤에 공백 유지
		if(usingExit == false) exitblock = "";
		Estring::replaceAll(stateblock, "%INI% ", ini); //뒤에 공백유지
		Estring::replaceAll(stateblock, "%SNAME%", sname);
		Estring::replaceAll(stateblock, "%TRANSS%", stateTransStr);
		Estring::replaceAll(stateblock, "%EXIT%", exitblock);
		Estring::replaceAll(stateblock, "%VARS%", monVars);
		Estring::replaceAll(stateblock, "%BHVNAME%", fsm->getRefBhv());
		stateinMon.append(stateblock).append("\n");
	}

	
	//if(prefix =="startBhv") 
	std::string bname2;
	if(isSCname(prefix))bname2 = fsm->getName();
	//else bname2 = fsm->getRefBhv() + "_MON_" + prefix;
	else bname2 = "SEL_" + prefix;
	Estring::replaceAll(fsmblock, "%BHVNAME%", bname2);
	Estring::replaceAll(fsmblock, "%STATES%", stateinMon);
	Estring::replaceAll(fsmblock, "%TNAME%", "---");
	Estring::replaceAll(fsmblock, "%FSMNAME%", fsm->getName());
	Estring::replaceAll(fsmblock, "%PREFIX%", prefix);
	fputs(fsmblock.c_str(), tfile);
}

void TPLGenerator::generateFSMCode2(FILE* tfile, std::string prefix, FSM4BS* fsm)
{
	//1. con
	//if(prefix != "startBhv"){
	if(!isSCname(prefix)){		
		std::string conblock = "";
		if(fsm->getMonOp() == "^" && fsm->isMonitorSelector()){
			//conexer가 필요없다.
		}
		else if(fsm->getMonOp() == "~" && fsm->isMonitorSelector()){
			//conexer
			conblock = getBlock("$con_block4~");
			Estring::replaceAll(conblock, "%ACT_PREFIX_VAR%", attachModelname(prefix));
		}

		Estring::replaceAll(conblock, "%FSMNAME%", fsm->getName());
		Estring::replaceAll(conblock, "%BHVNAME%", fsm->getRefBhv());
		Estring::replaceAll(conblock, "%PREFIX%", prefix);
		Estring::replaceAll(conblock, "%MON_EVT%", prefix);
		fputs(conblock.c_str(), tfile);
	}

	//2. mon
	//$fsm_block@$start_block
	//behavior %BHVNAME%(){
	//%STATES%
	//};
	std::string fsmblock;
	if(fsm->getMonOp() == "^" && fsm->isMonitorSelector()) 
		fsmblock = getBlock("$con_block4^");
	if(fsm->getMonOp() == "*" && fsm->isMonitorSelector()) 
		fsmblock = getBlock("$con_block4*");
	else 
		fsmblock = getBlock("$fsm_block");

	std::vector<NodeState*> states = fsm->getStates();
	std::string stateinMon;
	std::string monVars;
	for(unsigned int i=0; i<states.size();i++){
		std::string stateblock;
		if(fsm->isMonitorSelector() && fsm->getMonOp() == "^")
			//state 블록을 따로 만들지 않고 con_block4^블록의 entry와 transition을 채운다.
			stateblock = getBlock("$state_in_mon_selector^");
		else if(fsm->isMonitorSelector() && fsm->getMonOp() == "*") 
			stateblock = getBlock("$state_in_mon_selector*");
		else if(fsm->isMonitorSelector() && fsm->getMonOp() == "~") 
			stateblock = getBlock("$state_in_mon_selector~");
		else // 예외처리를 fsm
			stateblock = getBlock("$state_in_mon");
		std::string exitblock = getBlock("$exit_in_state");
		std::vector<NodeTrans*> transs = states[i]->getTrans();
		std::string stateid = states[i]->getBhvName();
		bool usingExit = false;


		std::string stateTransStr = getTransBlock(fsm, states[i]);
		std::string monVars;
		if(fsm->isMonitorSelector()) monVars = getMonitorVars(states[i]);
		

		//%INI% state %SNAME%{
		//	transition{
		//		%TRANSS%
		//	}
		//}
		std::string sname = states[i]->getState();
		std::string ini;
		if(states[i]->isInitial()) ini = "initial "; //뒤에 공백 유지
		if(usingExit == false) exitblock = "";
		Estring::replaceAll(stateblock, "%INI% ", ini); //뒤에 공백유지
		Estring::replaceAll(stateblock, "%SNAME%", sname);
		Estring::replaceAll(stateblock, "%TRANSS%", stateTransStr);
		Estring::replaceAll(stateblock, "%EXIT%", exitblock);
		Estring::replaceAll(stateblock, "%VARS%", monVars);
		Estring::replaceAll(stateblock, "%BHVNAME%", fsm->getRefBhv());
		stateinMon.append(stateblock).append("\n");
	}

	if(fsm->isMonitorSelector() && fsm->getMonOp() == "*" )
		stateinMon.append(getState4Trans(fsm, states[0]));

	
	//if(prefix =="startBhv") 
	std::string bname2;
	if(isSCname(prefix))bname2 = fsm->getName();
	//else bname2 = fsm->getRefBhv() + "_MON_" + prefix;
	else bname2 = "SEL_" + prefix;
	Estring::replaceAll(fsmblock, "%BHVNAME%", bname2);
	Estring::replaceAll(fsmblock, "%STATES%", stateinMon);
	Estring::replaceAll(fsmblock, "%TNAME%", "---");
	Estring::replaceAll(fsmblock, "%FSMNAME%", fsm->getName());
	Estring::replaceAll(fsmblock, "%PREFIX%", prefix);
	fputs(fsmblock.c_str(), tfile);
}

std::string TPLGenerator::getState4Trans(FSM4BS* fsm, NodeState* state)
{
	//예외처리하는 state 생성, *인경우에만 해당
	std::string statesStr;
	std::vector<NodeTrans*> transs = state->getTrans();
	for(unsigned int j=0; j<transs.size(); j++){
		std::string	tgname = transs[j]->getTransTarget();
		std::string state4trans = getBlock("$state_in_mon_selector2*");
		Estring::replaceAll(state4trans, "%TARGET%", tgname+"()");
		Estring::replaceAll(state4trans, "%SNAME%", tgname+"_state");
		statesStr.append(state4trans);
	}

	return statesStr;
}

std::string TPLGenerator::getTransBlock(FSM4BS* fsm, NodeState* state)
{
	std::string stateTransStr = "";
	std::vector<NodeTrans*> transs = state->getTrans();
	std::string stateid = state->getBhvName();

	NodeTrans* temp = NULL;
	if(fsm->isMonitorSelector() && fsm->getMonOp() == "~" ){
		//우선 동시실행 중인 모니터링 대상 behavior가 실행 끝났는지 체크
		//%IF_EVT% %TRANSTYPE% %TARGET% %NEXT%;
		stateTransStr = getBlock("$trans_in_state");
		Estring::replaceAll(stateTransStr, "%IF_EVT% ", "if("+attachModelname(fsm->getPrefix())+")");
		Estring::replaceAll(stateTransStr, "%TRANSTYPE%", " moveto");
		Estring::replaceAll(stateTransStr, "%TARGET%", "g");
		Estring::replaceAll(stateTransStr, "%NEXT%", "");
	}

	for(unsigned int j=0; j<transs.size(); j++){ //마지막 state가 아니면 transs가 1보다 크다
		unsigned int	ttype		= transs[j]->getTransType();
		std::string		tgname		= transs[j]->getTransTarget();
		std::string		srname		= transs[j]->getTransSource();
		std::string		nxt			= transs[j]->getStateAfterExpand();
		std::string		cond		= transs[j]->getCond();
		std::string		monPrefix	= transs[j]->getMonPrefix();

		std::string transblock;
		if(fsm->isMonitorSelector()){
			if(fsm->getMonOp() == "*")
				transblock = getBlock("$trans_in_state3");
			else
				transblock = getBlock("$trans_in_state2");
		}
		else
			//%IF_EVT% %TRANSTYPE% %TARGET% %NEXT%;
			transblock = getBlock("$trans_in_state");

		std::string evtExpr;
		std::string condExpr;
		std::string monCond;
		if(monPrefix != ""){
			//monPrefix는 fsm->isMonitorSelector()을 만족하는 fsm이아니라 selector를 호출하는 state의 transition에 존재함
			condExpr = attachModelname("SEL_" + monPrefix);
			if (cond!= "") condExpr.append(" && ");
		}
		//if(cond == "else") evtExpr = "else";
		if(cond != ""){
			condExpr.append(getEventExpr(cond));
		}
		if(cond != "" || monPrefix != "") evtExpr = "if("+ condExpr + ") ";
		if(j>0) evtExpr = "else " + evtExpr;
		
		Estring::replaceAll(transblock, "%IF_EVT% ", evtExpr);
		std::string ttypeStr = "expand";
		std::string bb = "()";
		if(ttype==2) {
			ttypeStr = "moveto";
			bb="";
		}

		if(fsm->isMonitorSelector()){//state가 하나밖에 없음
			//transition 블록의 if문에 들어갈 변수
			Estring::replaceAll(transblock, "%MONPREFIX_VAR%", attachModelname(tgname));
		}
		Estring::replaceAll(transblock, "%TRANSTYPE%", ttypeStr);
		Estring::replaceAll(transblock, "%TARGET%", tgname+bb);
		std::string nxt2;
		if(nxt != "") nxt2 = std::string("~>").append(nxt);
		Estring::replaceAll(transblock, "%NEXT%", nxt2);
		stateTransStr.append(transblock);
	}

	//FSM의 마지막 state는 trans가 없다.
	if(transs.size()<1){
		//%IF_EVT% %TRANSTYPE% %TARGET% %NEXT%;
		std::string transblock = getBlock("$trans_in_state");

		Estring::replaceAll(transblock, "%TRANSTYPE%", "expand");
		//std::string tgname = states[i]->getUname();
		std::string prefix = state->getPrefix();
		if(hasNestSeq(prefix))
			//Estring::replaceAll(tgname, "state_", "");
			Estring::replaceAll(transblock, "%TARGET%", stateid + "_CON_" + prefix + "()");
		else
			Estring::replaceAll(transblock, "%TARGET%", stateid + "()");

		Estring::replaceAll(transblock, "%NEXT%", "~> g");

		std::string evtExpr = "";
		if(state->getEvtRef() != "" ) evtExpr = "if("+getEventExpr(state->getEvtRef())+") ";
		Estring::replaceAll(transblock, "%IF_EVT% ", evtExpr);
		
		stateTransStr = transblock;
	}

	return stateTransStr;
}

std::string TPLGenerator::getMonitorVars(NodeState* state)
{
	//monitor selector의 entry블록에 들어가 변수 셋팅
	std::string monVarStr;

	std::vector<NodeTrans*> transs = state->getTrans();
	std::string stateid = state->getBhvName();
	
	for(unsigned int j=0; j<transs.size(); j++){ 
		std::string	tgname = transs[j]->getTransTarget();
		std::string varInSelector = getBlock("$var_in_selector");
		Estring::replaceAll(varInSelector, "%MONPREFIX_VAR%", attachModelname(tgname));
		monVarStr.append(varInSelector);
	}

	return monVarStr;
}

void TPLGenerator::generateModelCode(FILE* tfile)
{
	std::string model = getBlock("$model4_selector");

	std::set<std::string>::iterator iter;
	std::string vars;
	for(iter=m_modelvars.begin(); iter!=m_modelvars.end(); iter++){
		std::string modelvar = getBlock("$model_ele4_selector");
		Estring::replaceAll(modelvar, "%MONPREFIX_VAR%", *iter);
		vars.append(modelvar);
	}

	Estring::replaceAll(model, "%MODEL_NAME%", m_modelname);
	Estring::replaceAll(model, "%MODEL_ELES%", vars);

	fputs(model.c_str(), tfile);
}

void TPLGenerator::initBhvTable(NodeBS* bs, std::string bsname)
{
	if(typeid(*bs)==typeid(NodeCon)){
		NodeCon* con = (NodeCon*)bs;
		std::string conName = "con";
		conName.append("_").append(int2str(con->getNodeId()));
		con->setState(conName);
		std::string conblock = "$conexer_block";
	
		m_btIter = m_bhvTab.find(conName);	
		if(m_btIter == m_bhvTab.end()) { //bhv테이블에 없으면 삽입
			std::string* pTem = new std::string(getBlock(conblock));
			m_bhvTab.insert(pstrpair(conName, pTem));
		}

		std::vector<NodeBS*> cnodes = con->getSubNodes();
		for(unsigned int i=0; i<cnodes.size(); i++){
			initBhvTable(cnodes[i], bsname);
		}
	}
	else if(typeid(*bs)==typeid(NodeSeq)){
		NodeSeq* seq = (NodeSeq*)bs;

		std::string seqName = "seq";
		seqName.append("_").append(int2str(seq->getNodeId()));
		seq->setState(seqName);
		std::string seqblock = "$seqexer_block";

		m_btIter = m_bhvTab.find(seqName);	
		if(m_btIter == m_bhvTab.end()) { //bhv테이블에 없으면 삽입
			std::string* pTem = new std::string(getBlock(seqblock));
			m_bhvTab.insert(pstrpair(seqName, pTem));
		}

		std::vector<NodeBS*> cnodes = seq->getSubNodes();
		for(unsigned int i=0; i<cnodes.size(); i++){
			initBhvTable(cnodes[i], bsname);
			//seq일 때는 첫번째 서브노드의 이벤트를 seq의 이벤트로 설정
			std::string firstsub_evt = cnodes[0]->getEvtRef();
			if(firstsub_evt != ""){
				cnodes[0]->setEvtRef("");
				seq->setEvtRef(firstsub_evt);
			}
		}
	}
	else if(typeid(*bs)==typeid(NodeBhv)){
		NodeBhv* bhv = (NodeBhv*)bs;
		NodeBS* sub = bhv->getSubNode();

		std::string bhvname = bhv->getBhvRef();
		bhv->setState(bhvname);
		std::string bhvblock = "$bhv_block";	
	
		m_btIter = m_bhvTab.find(bhvname);	
		if(m_btIter == m_bhvTab.end()) { //bhv테이블에 없으면 삽입
			std::string* pTem = new std::string(getBlock(bhvblock));
			m_bhvTab.insert(pstrpair(bhvname, pTem));
		}

		if(isCycled(bhvname)){//사이클인 경우:가상 behavior가져오기
			bhvname = bhv->getBhvRef();
			bhvname.append("_").append(int2str(bhv->getNodeId()));
			bhv->setState(bhvname); //uname을 바꾼다.
			if(sub != NULL) 
				bhvblock = "$virtual_bhv_block";
			else //사이클이라도 leaf이면 state가 s1과 goal2개이다.
				bhvblock = "$virtual_bhv_block2";

			m_btIter = m_bhvTab.find(bhvname);	
			if(m_btIter == m_bhvTab.end()) { //bhv테이블에 없으면 삽입
				std::string* pTem = new std::string(getBlock(bhvblock));
				m_bhvTab.insert(pstrpair(bhvname, pTem));
			}
		}

		
		if(sub!=NULL) initBhvTable(sub, bsname);
	}
}

bool TPLGenerator::isCycled(std::string bhv)
{
	std::set<std::string>::iterator it = m_cycleBhvs.find(bhv);
	if(it != m_cycleBhvs.end()){
		return true;
	}
	return false;
}

NodeTrans* TPLGenerator::buildTrans(NodeState* sourceState, NodeState* targetState,  std::string evtRef)
{
	NodeTrans* trans = new NodeTrans();

	//1.Source 설정
	trans->setCond(evtRef);
	trans->setTransSource(sourceState->getState());

	//2. Target설정
	if(sourceState->isInitial()){
		//startBhv에서 state_bs_sc1의 transition이 여기에 해당됨
		trans->setTransTarget(MOVETO_, targetState->getState());
	}
	//nest를 가지면 C_CON, 
	else if(hasNestSeq(sourceState->getPrefix())){
		trans->setTransTarget(EXPAND_, sourceState->getBhvName() + "_CON_" + sourceState->getPrefix());
	}
	//아니면 souurce
	else{
		trans->setTransTarget(EXPAND_, sourceState->getBhvName());
	}
	
	// ~> 설정
	if(trans->getTransType()==EXPAND_){
		if(targetState != NULL)
			trans->setStateAfterExpand(targetState->getState());
		else
			trans->setStateAfterExpand("g");
	}

	//이미존재, 중복 삽입
	if(sourceState->addTrans(trans) == -1){
		delete trans;
		trans = NULL;
	}

	return trans;
}

void TPLGenerator::addStates4PostCon(FSM4BS* fsm)
{
	//buildUPFSM이후에
	//A->B 에서 A속에 bs state가 존재하면
	//아래와 같이 되도록 state와 trans를 삽입 및 변경
	//state state_C_CON_post{
	//	transition{
	//		if(!(sc3)) moveto state_D_CON ;
	//		moveto g ;
	//	}	
	//}


	//for(m_fsmTABIter=m_fsmTAB.begin(); m_fsmTABIter!=m_fsmTAB.end(); m_fsmTABIter++){
		//std::string bname = m_fsmTABIter->first;
		//FSM4BS* fsm = m_fsmTABIter->second;
		
		//if(fsm->getName() == "startBhv") return;

		std::vector<NodeState*> states = fsm->getStates();
		std::vector<NodeState*> newStates;
		for(unsigned int i=0; i<states.size();i++){
			std::string stateid = states[i]->getBhvName();
			if(hasNestSeq(states[i]->getPrefix())){ //C[A->B]->SC에서 C인 경우
				//std::vector<std::string> bss = getBSstates(stateid);
				std::string statename = states[i]->getBhvName();
				std::string po_Sname = statename + "_CON_post_" + states[i]->getPrefix();
				NodeState* po_state = new NodeState(states[i]->getEvtRef(), po_Sname, states[i]->getPrefix());
				po_state->setState("state_" + po_Sname);

				
				std::vector<NodeTrans*> trans = states[i]->getTrans();
				for(unsigned int j=0; j<trans.size(); j++){
					int ttype = trans[j]->getTransType();
					//std::string tg = trans[j]->getTransTarget();
					std::string nxtState = trans[j]->getStateAfterExpand();
					//state[i] 기존 trans를 po_state로 가도록 설정
					//trans[j]->setTransTarget(MOVETO_, po_Sname);
					trans[j]->setStateAfterExpand(po_state->getState());

					//po_state에 새로운 trans삽입
					NodeTrans* transInPstate = new NodeTrans();
					//std::vector<std::string> states4BS = getBSstates(states[i]->getPrefix());
					//transInPstate->setCond(getCond4NotBS(states4BS));
					transInPstate->setTransSource(po_state->getState());
					transInPstate->setTransTarget(MOVETO_, nxtState);
					po_state->addTrans(transInPstate);
					
				}
				/*			
				//else 삽입
				NodeTrans* transInPstate2 = new NodeTrans();
				//transInPstate2->setCond("else"); //else가 없어도 된다.
				transInPstate2->setEnclosingState(po_state->getState());
				transInPstate2->setTransTarget(MOVETO_, "g");
				po_state->addTrans(transInPstate2);
				*/	

				newStates.push_back(po_state);
	
			}
		}
		if(newStates.size()>0){
			for(unsigned int i=0; i<newStates.size();i++){
				fsm->addState(newStates[i]);
			}
		}
	//}
}


/*
아래와 같이 같은 prefix를 가지는 행위 A를 수행하고 이벤트에 따라 선택이 달라질 때
이벤트를 비교하는 state가 A다음에 필요하다
A->e1:B
A->e2:D

A->s1->(B,D)

하나의 state를 생성하고 임의의 지점에 삽입하는 함수로 바꾸자
*/
void TPLGenerator::addStates4Branch(FSM4BS* fsm)
{
	int key = 1;

	std::set<NodeState*> newStates;
	std::vector<NodeState*>	states = fsm->getStates();
	for(unsigned int i=0; i<states.size(); i++){
		//모호한 상황 예)	S->A->e1:B, S->A->e2:C, S->A->e3:D는 state A는 모호함
		if(states[i]->isAmbiguous()){
			std::string target4toS;
			unsigned int targetType4toS;

			//1. 새로운 state 생성
			std::string sname = "s" + int2str(key++); 
			std::string sprefix = states[i]->getPrefix() + "_" + sname;
			NodeState* newState = new NodeState("", sname, sprefix);
			newState->setState(sname);
			
			//2. states[i]는 소스, 여기에 포함된 각각의 transition에 대해서
			std::vector<NodeTrans*>	trans = states[i]->getTrans();
			for(unsigned int j=0; j<trans.size(); j++){
				target4toS		= trans[j]->getTransTarget();
				targetType4toS	= trans[j]->getTransType();
				std::string	monPrefix = trans[j]->getMonPrefix();

				//2.1 state[i]의 target state의 이름 획득
				std::string tname = "";
				if(trans[j]->getTransType() == EXPAND_)
					tname = trans[j]->getStateAfterExpand();
				else //MOVETO_
					tname = trans[j]->getTransTarget();
				
				//2.2 target state가져오기
				NodeState* tstate = fsm->getState(tname);

				//2.3 target state를 얻은 다음 newState에 포함될 transition생성
				if(tstate != NULL){ //lsname이 g 등인 경우 NULL값이다.
					//std::string ls_new_prefix = sprefix + tstate->getEvtBhvStrInPrefix();
					//tstate->setPrefix(ls_new_prefix);
					
					//2.3.1 새로운 state에 삽일된 trans 생성
					std::vector<NodeTrans*>	transInTarget = tstate->getTrans();
					for(unsigned int k=0; k<transInTarget.size(); k++){
						NodeTrans* newTrans = new NodeTrans();
						//std::string cond = transInTarget[k]->getCond();
						//std::string monPrefix = transInTarget[k]->getMonPrefix();

						newTrans->setCond(transInTarget[k]->getCond());
						newTrans->setMonPrefix(monPrefix);
						newTrans->setTransSource(sname);
						newTrans->setTransTarget(MOVETO_, transInTarget[k]->getTransSource());
						newState->addTrans(newTrans);

						transInTarget[k]->setCond("");
					}
				}
			}
			
			//4. 새로운 state에 잠시 저장
			newStates.insert(newState);

			//5. 원래 state[i]의 trans 삭제
			states[i]->deleteAllTrans();

			//6. state[i]에 새롭게 저장될 trans
			NodeTrans* toS = new NodeTrans();
			toS->setTransTarget(targetType4toS, target4toS);
			toS->setTransSource(states[i]->getBhvName());
			toS->setStateAfterExpand(sname);
			states[i]->addTrans(toS);
		}
	}
	//7. 새롭게 생성된 state를 fsm에 추가
	std::set<NodeState*>::iterator it = newStates.begin();
	for(;it!=newStates.end(); it++){
		fsm->addState(*it);
	}
	newStates.clear();
}

void TPLGenerator::addInitialState(FSM4BS* fsm)
{
	//fsm의 state들의 prefix들과 fsm의 prefix를 비교해서 start state를 알 수 있다.
	std::set<NodeState*> startStates;
	std::vector<NodeState*>	states = fsm->getStates();
	for(unsigned int i=0; i<states.size(); i++){
		if(states[i]->isStartState(fsm->getPrefix()))
			startStates.insert(states[i]);
	}
	

/*
	std::set<NodeState*>::iterator it = startStates.begin();
	std::string monOp = fsm->getMonOp();
	if(monOp == "~"){
		std::string sname = "s0"; 
		std::string sprefix = (*it)->getPrefix() + "_" +sname;
		NodeState* newInitState = new NodeState("", sname, sprefix);
		newInitState->setState(sname);
		newInitState->setAsInitial(true);

		std::string monEvt = fsm->getMonEvt();
		return;
	}
*/	
	std::set<NodeState*>::iterator it = startStates.begin();
	if(startStates.size()>1){
		std::string sname = "s0"; 
		std::string sprefix = (*it)->getPrefix() + "_" +sname;
		NodeState* newInitState = new NodeState("", sname, sprefix);
		newInitState->setState(sname);
		newInitState->setAsInitial(true);
		for(;it!=startStates.end(); it++){
			//initial state에 trans생성
			buildTrans(newInitState, (*it), (*it)->getEvtRef());
			//(*it)의 evtRef와 관련 trans의 조건을 제거
			(*it)->setEvtRef("");
			std::vector<NodeTrans*>	trans = (*it)->getTrans();
			for(unsigned int j=0; j<trans.size(); j++){
				if((*it)->getBhvName() == trans[j]->getTransTarget()){
					trans[j]->setCond("");
				}
			}
		}
		fsm->addState(newInitState);
	}
	if(startStates.size()==1){
		(*it)->setAsInitial(true);
	}

	//fsm과 같은 이름의 bhv를 initial state로 moveto하게 설정
	std::string is = fsm->getInitialState();
	std::string fsmname = fsm->getName();
	for(unsigned int i=0; i<states.size(); i++){
		if(states[i]->getBhvName() == fsmname){
			std::vector<NodeTrans*> trans = states[i]->getTrans();
			for(unsigned int j=0; j<trans.size(); j++){
				trans[j]->setTransTarget(MOVETO_, is);
				trans[j]->setStateAfterExpand("");
			}
		}
	}

}


void TPLGenerator::addStates4Bhvs(std::map<std::string, std::set<std::string>>& dupBhvs, FSM4BS* fsm, NodeBS* ps, std::string bsname, NodeBS* bs/*, std::string bsRef*/)
{
	std::string fsmName = fsm->getName();

	if(typeid(*bs)==typeid(NodeBhv)){
		//state관련 이름 설정
		NodeBhv* bhv = (NodeBhv*)bs;
		std::string bname = bhv->getBhvRef();
		std::string uname = getStateUName4ExpandBhv(bhv);
		if(!isSCname(bname) && getNumOfPrefix(bname, dupBhvs)>1)
			uname.append("_").append(bhv->getPrefix());
		bhv->setState(uname);
		//state생성 및 추가
		NodeState* expand2con = new NodeState(bhv->getEvtRef(), bname, bhv->getPrefix());
		expand2con->setState(uname);
		
		//monitor를 포함하는 behavior를 expand하는 state인 경우 값이 할당됨
//expand2con->setMonEvt(bhv->getSubMonEvt());
//expand2con->setMonOp(bhv->getSubMonOp());

		//expand2con->setPrefix(bhv->getPrefix());
		fsm->addState(expand2con);
	}

	else if(typeid(*bs)==typeid(NodeCon)){
		NodeCon* con = (NodeCon*)bs;
		std::vector<NodeBS*> cnodes = con->getSubNodes();
		for(unsigned int i=0; i<cnodes.size(); i++){
		
		}
	}
	//B[e:A->D->C] 일 때 
	else if(typeid(*bs)==typeid(NodeSeq)){
		NodeSeq* seq = (NodeSeq*)bs;
		std::vector<NodeBS*> cnodes = seq->getSubNodes();

		//1. seq에 포함된 state를 생성
		for(unsigned int i=0; i<cnodes.size(); i++){
			addStates4Bhvs(dupBhvs, fsm, seq, bsname, cnodes[i]);
		}

		//2. Transtion생성(State를 모두 생성하였다면 다음으로 transition생성)
		for(unsigned int j = 0; j<cnodes.size()-1; j++){
			NodeState* state1 = fsm->getState(cnodes[j]->getState());
			NodeState* state2 = fsm->getState(cnodes[j+1]->getState());

			
			std::string evtRef = cnodes[j]->getEvtRef();
			NodeTrans* newTrans = buildTrans(state1, state2, evtRef);

			//예외처리 다음에 NodeBS가 오는 경우
			if(newTrans != NULL && typeid(*cnodes[j])==typeid(NodeBhv)){
				NodeBhv* bhv = (NodeBhv*)cnodes[j];
				std::string monEvt	= bhv->getSubMonEvt();
				std::string monOp	= bhv->getSubMonOp();
				if(monEvt != ""){
					newTrans->setMonPrefix(state1->getPrefix()+getMonPostfix(bhv));
					//m_modelvars.insert(state1->getPrefix()+getMonPostfix(bhv));
				}
			}

			if(j+1 == cnodes.size()-1)
				buildTrans(state2, NULL, cnodes[j+1]->getEvtRef());
		}
	}
}


bool TPLGenerator::isLeaf(NodeBS* node)
{
	if(typeid(*node)==typeid(NodeBhv)){
		NodeBhv* bhv = (NodeBhv*) node;
		if(bhv->getSubNode()== NULL)
			return true;
	}
	return false;
}

/**
임의의 state가 예외처리를 가지고 있는 behavior를 호출하고 있는 지를 체크
*/
/*
bool TPLGenerator::isState4ExptMon(NodeState* state)
{
	std::string sPrefix = state->getPrefix();
	for(m_fsmTABIter=m_fsmTAB.begin(); m_fsmTABIter!=m_fsmTAB.end(); m_fsmTABIter++){
		FSM4BS* fsm	= m_fsmTABIter->second;
		std::string mtPrefix = fsm->getPrefix4MonTarget();
		if(mtPrefix == sPrefix)
			return true;
	}
	return false;
}
*/

std::string TPLGenerator::getStateUName4ExpandBhv(NodeBhv* bhv)
{
	if(hasNestSeq(bhv->getPrefix()))
		return "state_" + bhv->getBhvRef() + "_CON";
	else
		return "state_" + bhv->getBhvRef();
}

bool TPLGenerator::hasNestSeq(std::string prefix)
{
	std::map<std::string, FSM4BS*>::iterator it = m_fsmTAB.find(prefix);
	if(it != m_fsmTAB.end())//nest behavior를 포함
		return true;
	else
		return false;
}

bool TPLGenerator::isSCname(std::string stateId)
{
	for(unsigned int i=0; i<m_sclist.size(); i++){
		if(m_sclist[i]->getSCname() == stateId)
			return true;
	}
	return false;
}

std::string TPLGenerator::getStartStateOfBS(std::vector<SBhvSeq*> bslist, std::string bsid)
{
	for(unsigned int i=0; i<bslist.size(); i++){
		if(bslist[i]->getName()->getData()==bsid){
			NodeBhv* firstBhv = findFirstBhvNode(bslist[i]->getStartNode());
			if(firstBhv != NULL)
				return firstBhv->getState();
		}
	}
	return "";
}

std::string TPLGenerator::attachModelname(std::string monPrefix)
{
	//이함수에 호출된 monPrefix가 사용되는 변수이다
	std::string varname = monPrefix + "_var";
	m_modelvars.insert(varname);
	return m_modelname + "." + varname;
}

NodeBhv* TPLGenerator::findFirstBhvNode(NodeBS* node)
{
	if(typeid(*node)==typeid(NodeBhv))
		return (NodeBhv*)node;
	else if(typeid(*node)==typeid(NodeSeq)){
		NodeSeq* seq = (NodeSeq*)node;
		std::vector<NodeBS*> subs = seq->getSubNodes();
		if(subs.size()>0) 
			return findFirstBhvNode(subs[0]);
	}
	return NULL;
}

void TPLGenerator::addEvtNode4Monitor(NodeBhv* bhv)
{
	NodeBS* sub = bhv->getSubNode();

	if(sub != NULL){
		std::string monOp  = bhv->getSubMonOp();
		std::string monEvt = bhv->getSubMonEvt();

		if(monOp == "~"){
			std::vector<NodeBS*> subsublist = ((NodeSeq*)sub)->getSubNodes();
			if(subsublist.size()>0){
				//subsublist[0]->setEvtRef(monEvt);
			}
		}
		if(monOp == "^"){
			NodeTrans* trans = new NodeTrans();
			trans->setCond(monEvt);
			//trans->setEnclosingState();
			//trans->setStateAfterExpand();
			//trans->setTransTarget();
		}
	}
}

void TPLGenerator::makeFSM(NodeBS* bs, std::string type)
{
	if(typeid(*bs)==typeid(NodeCon)){
		NodeCon* con = (NodeCon*)bs;
		std::vector<NodeBS*> cnodes = con->getSubNodes();
		for(unsigned int i=0; i<cnodes.size(); i++){
			makeFSM(cnodes[i], type);
		}
	}
	else if(typeid(*bs)==typeid(NodeSeq)){
		NodeSeq* seq = (NodeSeq*)bs;
		std::vector<NodeBS*> cnodes = seq->getSubNodes();
		for(unsigned int i=0; i<cnodes.size(); i++){
			makeFSM(cnodes[i], type);
		}
	}
	else if(typeid(*bs)==typeid(NodeBhv)){
		NodeBhv* bhv = (NodeBhv*)bs;
		//std::string evt = bhv->getEvtRef();
		std::string refBhv = bhv->getBhvRef();
		//std::string newPrefix = prefix;
		//if(evt != "") newPrefix.append("_").append(evt);
		//newPrefix.append("_").append(bname);
		//bhv->setPrefix(newPrefix);

		//서브노드가 있으면 서브 노드에 대한 FSM을 생성한다.
		NodeBS* sub = bhv->getSubNode();
		std::string monOp  = bhv->getSubMonOp(); //""문자일 수도 있음
		std::string monEvt = bhv->getSubMonEvt();

		FSM4BS* fsm = NULL;
		if(sub != NULL){
			std::string prefix = sub->getPrefix();
			std::map<std::string, FSM4BS*>::iterator it = m_fsmTAB.find(prefix);
			if(it != m_fsmTAB.end()){
				//FSM은 다수의 BhvSeq를 가질 수 있다.
				fsm = it->second;
				fsm->addBhvSeq(sub);
			}
			else{
				//FSM을 생성할 때 3가지 함수를 수행
				std::string fsmName = refBhv;
				fsmName.append("_CON");
				fsm = new FSM4BS(prefix, refBhv, fsmName, type);
				if(monOp != "") fsm->setPrefix4MonTarget(bhv->getPrefix());
				fsm->addBhvSeq(sub);
				m_fsmTAB.insert(m_fsmTABPair(prefix, fsm));
			}

			//fsm에 ~, ^, *를 저장
			fsm->setMonOp(monOp);
			fsm->setMonEvt(monEvt);
			addEvtNode4Monitor(bhv);
			makeFSM(sub, type);
		}
	}
}

void TPLGenerator::makeFSM4ExptMon()
{
	//1. 생성될 fsm대상을 뽑아낸다.
	std::set<std::string> prefix4monTarget;
	for(m_fsmTABIter=m_fsmTAB.begin(); m_fsmTABIter!=m_fsmTAB.end(); m_fsmTABIter++){
		//std::string prefix	= m_fsmTABIter->first;
		FSM4BS*		fsm		= m_fsmTABIter->second;
		std::string pfx4mt  = fsm->getPrefix4MonTarget();
		if(pfx4mt != ""){ 
			pfx4mt = pfx4mt + fsm->getMonOp();
			prefix4monTarget.insert(pfx4mt);
		}
	}

	//2. ~ 에서 예외처리 event를 입력받아 이를 처리하는 fsm을 선택하는 fsm생성
	std::set<std::string>::iterator p4mtIter;
	for(p4mtIter=prefix4monTarget.begin(); p4mtIter!=prefix4monTarget.end(); p4mtIter++){
		std::string prefix_op	= (*p4mtIter);
		std::string prefix		= prefix_op.substr(0, prefix_op.size()-1);
		std::string monop		= prefix_op.substr(prefix_op.size()-1);

		std::string refBhv = prefix.substr(prefix.find_last_of("_")+1, prefix.size());
		std::string fsmName = refBhv + "_CON";
		NodeState* s0 = new NodeState("", "s0", prefix+"_s0");
		s0->setAsInitial(true);
		FSM4BS* fsm = new FSM4BS(prefix, refBhv, fsmName, "");
		fsm->addState(s0);
		fsm->setMonOp(monop);
		fsm->setAsMonitorSelector();

		for(m_fsmTABIter=m_fsmTAB.begin(); m_fsmTABIter!=m_fsmTAB.end(); m_fsmTABIter++){
			FSM4BS* fsm	= m_fsmTABIter->second;
			//state s0에 transition을 생성
			if(fsm->getPrefix4MonTarget() == prefix){
				std::string expandEvt = fsm->getMonEvt();
				std::string expandTarget = fsm->getPrefix();
				NodeTrans* trans = new NodeTrans();
				trans->setCond(expandEvt);
				trans->setTransTarget(EXPAND_, "SEL_" + expandTarget);
				trans->setStateAfterExpand("g");
				s0->addTrans(trans);
			}
		}

		m_fsmTAB.insert(m_fsmTABPair(prefix, fsm));
	}
}

void TPLGenerator::setPrefix(std::string prefix, NodeBS* bs)
{
	if(typeid(*bs)==typeid(NodeCon)){
		NodeCon* con = (NodeCon*)bs;
		std::vector<NodeBS*> cnodes = con->getSubNodes();
		for(unsigned int i=0; i<cnodes.size(); i++){
			setPrefix(prefix, cnodes[i]);
		}
	}
	else if(typeid(*bs)==typeid(NodeSeq)){
		NodeSeq* seq = (NodeSeq*)bs;
		seq->setPrefix(prefix);
		std::vector<NodeBS*> cnodes = seq->getSubNodes();
		std::string newPrefix = prefix;
		for(unsigned int i=0; i<cnodes.size(); i++){
			setPrefix(newPrefix, cnodes[i]);
			newPrefix = cnodes[i]->getPrefix();
		}
	}
	else if(typeid(*bs)==typeid(NodeBhv)){
		NodeBhv* bhv = (NodeBhv*)bs;
		std::string newPrefix = getNewPrefix(prefix, bhv);
		bhv->setPrefix(newPrefix);
		NodeBS* sub = bhv->getSubNode();
		if(sub != NULL){
			//std::string newPrefix2 = getNewPrefix(prefix, sub);
			//~, ^, *에 대한 모니터인경우
			std::string monPostfix = getMonPostfix(bhv);
			newPrefix.append(monPostfix);
			setPrefix(newPrefix, sub);
		}
	}
}

std::string TPLGenerator::getMonPostfix(NodeBhv* bhv)
{
	std::string postfix;

	std::string monOp = bhv->getSubMonOp();
	std::string monEvt = bhv->getSubMonEvt();
	if(monOp == "~") postfix.append("_1_");
	if(monOp == "^") postfix.append("_2_");
	if(monOp == "*") postfix.append("_3_");
	if(monEvt != "") postfix.append(monEvt);

	return postfix;
}

std::string TPLGenerator::getNewPrefix(std::string prefix, NodeBS* bs)
{
	if(typeid(*bs)==typeid(NodeBhv)){
		NodeBhv* bhv = (NodeBhv*)bs;
		std::string evt = bhv->getEvtRef();
		std::string bname = bhv->getBhvRef();
		std::string newPrefix = prefix;
		if(evt != "") newPrefix.append("_").append(evt);
		newPrefix.append("_").append(bname);
		return newPrefix;
	}
	return prefix;
}

void TPLGenerator::findDuplicatedBhv(NodeBS* bs, std::set<std::string>& appearedList, std::map<std::string, std::set<std::string>>& duplicatedList)
{
	if(typeid(*bs)==typeid(NodeCon)){
		NodeCon* con = (NodeCon*)bs;
		std::vector<NodeBS*> cnodes = con->getSubNodes();
		for(unsigned int i=0; i<cnodes.size(); i++){
			
		}
	}
	else if(typeid(*bs)==typeid(NodeSeq)){
		NodeSeq* seq = (NodeSeq*)bs;
		std::vector<NodeBS*> cnodes = seq->getSubNodes();
		for(unsigned int i=0; i<cnodes.size(); i++){
			if(typeid(*cnodes[i])==typeid(NodeCon)){
			}
			else if(typeid(*cnodes[i])==typeid(NodeSeq)){
			}
			else if(typeid(*cnodes[i])==typeid(NodeBhv)){
				NodeBhv* bhv = (NodeBhv*)cnodes[i];
				updateDuplicatedList(bhv, appearedList, duplicatedList);
			}
		}
	}
	else if(typeid(*bs)==typeid(NodeBhv)){
		NodeBhv* bhv = (NodeBhv*)bs;
		updateDuplicatedList(bhv, appearedList, duplicatedList);
	}
}

void TPLGenerator::updateDuplicatedList(NodeBhv* bhv, std::set<std::string>& appearedList, std::map<std::string, std::set<std::string>>& duplicatedList)
{
	std::string bname = bhv->getBhvRef();
	std::string prefix = bhv->getPrefix();

	//if(isIn(bname, appearedList)){
		m_duplistIter = duplicatedList.find(bname);
		if(m_duplistIter != duplicatedList.end()){
			m_duplistIter->second.insert(bhv->getPrefix());
		}
		else{
			std::set<std::string> prefixSet;
			prefixSet.insert(bhv->getPrefix());
			duplicatedList.insert(m_duplistPair(bname, prefixSet));
		}
		//duplicatedList.insert(bname);
	//}
	appearedList.insert(bname);
}

bool TPLGenerator::isIn(std::string ele, std::set<std::string>& myset)
{
	std::set<std::string>::iterator it = myset.find(ele);
	if(it != myset.end()){//존재함
		return true;
	}
	return false;
}

int TPLGenerator::getNumOfPrefix(std::string bname, std::map<std::string, std::set<std::string>>& duplicatedList)
{
	m_duplistIter = duplicatedList.find(bname);
	if(m_duplistIter != duplicatedList.end()){
		return m_duplistIter->second.size();
	}
	return 0;
}

void TPLGenerator::detectCycle(NodeBS* bs, std::vector<std::string> ancestor)
{
	if(typeid(*bs)==typeid(NodeCon)){
		NodeCon* con = (NodeCon*)bs;
		std::vector<NodeBS*> cnodes = con->getSubNodes();
		for(unsigned int i=0; i<cnodes.size(); i++){
			std::vector<std::string> l = ancestor;
			detectCycle(cnodes[i], l);
		}
	}
	else if(typeid(*bs)==typeid(NodeSeq)){
		NodeSeq* seq = (NodeSeq*)bs;
		std::vector<NodeBS*> cnodes = seq->getSubNodes();
		for(unsigned int i=0; i<cnodes.size(); i++){
			std::vector<std::string> l = ancestor;
			detectCycle(cnodes[i], l);
		}
	}
	else if(typeid(*bs)==typeid(NodeBhv)){
		NodeBhv* bhv = (NodeBhv*)bs;
		std::string bname = bhv->getBhvRef();
		for(unsigned int i=0; i<ancestor.size(); i++){
			std::string p = ancestor[i];
			if(bname == p){
				bhv->setAsCycled();
				m_cycleBhvs.insert(p);
			}
		}

		ancestor.push_back(bname);
		NodeBS* sub = bhv->getSubNode();
		if(sub != NULL) detectCycle(sub, ancestor);
	}
}


std::string TPLGenerator::getEventExpr(std::string ename)
{	
	for(unsigned int i=0; i<m_sevtlist.size(); i++){
		if(m_sevtlist[i]->getName()->getData() == ename)
			return m_sevtlist[i]->getCondStr();
	}
	return ename;
}

std::string TPLGenerator::getBehaviorActions(std::string bname)
{
	for(unsigned int i=0; i<m_sbhvlist.size(); i++){
		if(m_sbhvlist[i]->getName()->getData() == bname){
			std::string bhvcode = m_sbhvlist[i]->getStmtStr();
			Estring::replaceAll(bhvcode, "$", "");
			Estring::trim(bhvcode);
			return bhvcode;
		}

	}

	return "";
}