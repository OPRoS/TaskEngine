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
	m_inclStr	= inclStr; //include����Ʈ

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

	//m_stasks, m_sclist,	m_sbhvlist, m_sevtlist���� parsed result���� �����ȴ�.
}

void TPLGenerator::generateTPL(std::string styleFileDir)
{
	//1. ������ !scenario��Ͽ� ���ؼ�
	for(unsigned int x=0; x<m_sclist.size(); x++){
		//std::string start = "startBhv";
		std::string start = m_sclist[x]->getSCname();
		std::string type = m_sclist[x]->getType();
		std::vector<SBhvSeq*> bslist = m_sclist[x]->getBSlist();
		
		if(bslist.size()<1) return;

		//@goto {begin, end}�� ���� ��� ����
		for(unsigned int i=0; i<bslist.size(); i++){
			SBhvSeq* bs = bslist[i];
			NodeBS* node = bs->getStartNode();
			setPrefix(start, node);
			bs->makeGotoBhv(start); 
		}
		
		//������ !scenario�� ���ؼ� FSM(Behavior) ����
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

	//2. FSM�� state�� ä���ֱ�
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



	//3. ���Ͽ� ����
	//initStyle("style/scenario.style");
	initStyle(styleFileDir + "/scenario.style");

	std::string tfilename = m_homeDir + "/" + m_scfname;
	Estring::replaceAll(tfilename, ".sc", ""); //scȮ���ڰ� �ƴѰ�� �̷��� ���ϸ� �ó����������� ���
	tfilename.append(".tpl");
	FILE* tfile = fopen(tfilename.c_str(),"w");
	if(tfile == NULL){
		//���� ��������, ���ϸ��� ������ ���ؼ� write�� ������ �������� ����
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
			//conexer�� �ʿ����.
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
			//state ����� ���� ������ �ʰ� con_block4^����� entry�� transition�� ä���.
			stateblock = getBlock("$state_in_mon_selector^");
		else if(fsm->isMonitorSelector() && fsm->getMonOp() == "~") 
			stateblock = getBlock("$state_in_mon_selector~");
		else // ����ó���� fsm
			stateblock = getBlock("$state_in_mon");
		std::string exitblock = getBlock("$exit_in_state");
		std::vector<NodeTrans*> transs = states[i]->getTrans();
		std::string stateid = states[i]->getBhvName();
		std::string stateTransStr;
		bool usingExit = false;
		
		
		for(unsigned int j=0; j<transs.size(); j++){ //������ state�� �ƴϸ� transs�� 1���� ũ��
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
				//monPrefix�� fsm->isMonitorSelector()�� �����ϴ� fsm�̾ƴ϶� selector�� ȣ���ϴ� state�� transition�� ������
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

			if(fsm->isMonitorSelector()){//state�� �ϳ��ۿ� ����
				//monitor selector�� entry��Ͽ� �� ���� ����
				std::string varInSelector = getBlock("$var_in_selector");
				Estring::replaceAll(varInSelector, "%MONPREFIX_VAR%", attachModelname(tgname));
				monVars.append(varInSelector);
			

				//transition ����� if���� �� ����
				Estring::replaceAll(transblock, "%MONPREFIX_VAR%", attachModelname(tgname));
			}
			Estring::replaceAll(transblock, "%TRANSTYPE%", ttypeStr);
			Estring::replaceAll(transblock, "%TARGET%", tgname+bb);
			std::string nxt2;
			if(nxt != "") nxt2 = std::string("~>").append(nxt);
			Estring::replaceAll(transblock, "%NEXT%", nxt2);
			stateTransStr.append(transblock);
		}
	

		//FSM�� ������ state�� trans�� ����.
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

		//EXIT���
		/*
		if(prefix != "startBhv" && isBSname(stateid)){
			//behavior sequence�� ��� �������� ����־ startBhv������ exit�� ������� ���� 
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
		if(states[i]->isInitial()) ini = "initial "; //�ڿ� ���� ����
		if(usingExit == false) exitblock = "";
		Estring::replaceAll(stateblock, "%INI% ", ini); //�ڿ� ��������
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
			//conexer�� �ʿ����.
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
			//state ����� ���� ������ �ʰ� con_block4^����� entry�� transition�� ä���.
			stateblock = getBlock("$state_in_mon_selector^");
		else if(fsm->isMonitorSelector() && fsm->getMonOp() == "*") 
			stateblock = getBlock("$state_in_mon_selector*");
		else if(fsm->isMonitorSelector() && fsm->getMonOp() == "~") 
			stateblock = getBlock("$state_in_mon_selector~");
		else // ����ó���� fsm
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
		if(states[i]->isInitial()) ini = "initial "; //�ڿ� ���� ����
		if(usingExit == false) exitblock = "";
		Estring::replaceAll(stateblock, "%INI% ", ini); //�ڿ� ��������
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
	//����ó���ϴ� state ����, *�ΰ�쿡�� �ش�
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
		//�켱 ���ý��� ���� ����͸� ��� behavior�� ���� �������� üũ
		//%IF_EVT% %TRANSTYPE% %TARGET% %NEXT%;
		stateTransStr = getBlock("$trans_in_state");
		Estring::replaceAll(stateTransStr, "%IF_EVT% ", "if("+attachModelname(fsm->getPrefix())+")");
		Estring::replaceAll(stateTransStr, "%TRANSTYPE%", " moveto");
		Estring::replaceAll(stateTransStr, "%TARGET%", "g");
		Estring::replaceAll(stateTransStr, "%NEXT%", "");
	}

	for(unsigned int j=0; j<transs.size(); j++){ //������ state�� �ƴϸ� transs�� 1���� ũ��
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
			//monPrefix�� fsm->isMonitorSelector()�� �����ϴ� fsm�̾ƴ϶� selector�� ȣ���ϴ� state�� transition�� ������
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

		if(fsm->isMonitorSelector()){//state�� �ϳ��ۿ� ����
			//transition ����� if���� �� ����
			Estring::replaceAll(transblock, "%MONPREFIX_VAR%", attachModelname(tgname));
		}
		Estring::replaceAll(transblock, "%TRANSTYPE%", ttypeStr);
		Estring::replaceAll(transblock, "%TARGET%", tgname+bb);
		std::string nxt2;
		if(nxt != "") nxt2 = std::string("~>").append(nxt);
		Estring::replaceAll(transblock, "%NEXT%", nxt2);
		stateTransStr.append(transblock);
	}

	//FSM�� ������ state�� trans�� ����.
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
	//monitor selector�� entry��Ͽ� �� ���� ����
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
		if(m_btIter == m_bhvTab.end()) { //bhv���̺� ������ ����
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
		if(m_btIter == m_bhvTab.end()) { //bhv���̺� ������ ����
			std::string* pTem = new std::string(getBlock(seqblock));
			m_bhvTab.insert(pstrpair(seqName, pTem));
		}

		std::vector<NodeBS*> cnodes = seq->getSubNodes();
		for(unsigned int i=0; i<cnodes.size(); i++){
			initBhvTable(cnodes[i], bsname);
			//seq�� ���� ù��° �������� �̺�Ʈ�� seq�� �̺�Ʈ�� ����
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
		if(m_btIter == m_bhvTab.end()) { //bhv���̺� ������ ����
			std::string* pTem = new std::string(getBlock(bhvblock));
			m_bhvTab.insert(pstrpair(bhvname, pTem));
		}

		if(isCycled(bhvname)){//����Ŭ�� ���:���� behavior��������
			bhvname = bhv->getBhvRef();
			bhvname.append("_").append(int2str(bhv->getNodeId()));
			bhv->setState(bhvname); //uname�� �ٲ۴�.
			if(sub != NULL) 
				bhvblock = "$virtual_bhv_block";
			else //����Ŭ�̶� leaf�̸� state�� s1�� goal2���̴�.
				bhvblock = "$virtual_bhv_block2";

			m_btIter = m_bhvTab.find(bhvname);	
			if(m_btIter == m_bhvTab.end()) { //bhv���̺� ������ ����
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

	//1.Source ����
	trans->setCond(evtRef);
	trans->setTransSource(sourceState->getState());

	//2. Target����
	if(sourceState->isInitial()){
		//startBhv���� state_bs_sc1�� transition�� ���⿡ �ش��
		trans->setTransTarget(MOVETO_, targetState->getState());
	}
	//nest�� ������ C_CON, 
	else if(hasNestSeq(sourceState->getPrefix())){
		trans->setTransTarget(EXPAND_, sourceState->getBhvName() + "_CON_" + sourceState->getPrefix());
	}
	//�ƴϸ� souurce
	else{
		trans->setTransTarget(EXPAND_, sourceState->getBhvName());
	}
	
	// ~> ����
	if(trans->getTransType()==EXPAND_){
		if(targetState != NULL)
			trans->setStateAfterExpand(targetState->getState());
		else
			trans->setStateAfterExpand("g");
	}

	//�̹�����, �ߺ� ����
	if(sourceState->addTrans(trans) == -1){
		delete trans;
		trans = NULL;
	}

	return trans;
}

void TPLGenerator::addStates4PostCon(FSM4BS* fsm)
{
	//buildUPFSM���Ŀ�
	//A->B ���� A�ӿ� bs state�� �����ϸ�
	//�Ʒ��� ���� �ǵ��� state�� trans�� ���� �� ����
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
			if(hasNestSeq(states[i]->getPrefix())){ //C[A->B]->SC���� C�� ���
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
					//state[i] ���� trans�� po_state�� ������ ����
					//trans[j]->setTransTarget(MOVETO_, po_Sname);
					trans[j]->setStateAfterExpand(po_state->getState());

					//po_state�� ���ο� trans����
					NodeTrans* transInPstate = new NodeTrans();
					//std::vector<std::string> states4BS = getBSstates(states[i]->getPrefix());
					//transInPstate->setCond(getCond4NotBS(states4BS));
					transInPstate->setTransSource(po_state->getState());
					transInPstate->setTransTarget(MOVETO_, nxtState);
					po_state->addTrans(transInPstate);
					
				}
				/*			
				//else ����
				NodeTrans* transInPstate2 = new NodeTrans();
				//transInPstate2->setCond("else"); //else�� ��� �ȴ�.
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
�Ʒ��� ���� ���� prefix�� ������ ���� A�� �����ϰ� �̺�Ʈ�� ���� ������ �޶��� ��
�̺�Ʈ�� ���ϴ� state�� A������ �ʿ��ϴ�
A->e1:B
A->e2:D

A->s1->(B,D)

�ϳ��� state�� �����ϰ� ������ ������ �����ϴ� �Լ��� �ٲ���
*/
void TPLGenerator::addStates4Branch(FSM4BS* fsm)
{
	int key = 1;

	std::set<NodeState*> newStates;
	std::vector<NodeState*>	states = fsm->getStates();
	for(unsigned int i=0; i<states.size(); i++){
		//��ȣ�� ��Ȳ ��)	S->A->e1:B, S->A->e2:C, S->A->e3:D�� state A�� ��ȣ��
		if(states[i]->isAmbiguous()){
			std::string target4toS;
			unsigned int targetType4toS;

			//1. ���ο� state ����
			std::string sname = "s" + int2str(key++); 
			std::string sprefix = states[i]->getPrefix() + "_" + sname;
			NodeState* newState = new NodeState("", sname, sprefix);
			newState->setState(sname);
			
			//2. states[i]�� �ҽ�, ���⿡ ���Ե� ������ transition�� ���ؼ�
			std::vector<NodeTrans*>	trans = states[i]->getTrans();
			for(unsigned int j=0; j<trans.size(); j++){
				target4toS		= trans[j]->getTransTarget();
				targetType4toS	= trans[j]->getTransType();
				std::string	monPrefix = trans[j]->getMonPrefix();

				//2.1 state[i]�� target state�� �̸� ȹ��
				std::string tname = "";
				if(trans[j]->getTransType() == EXPAND_)
					tname = trans[j]->getStateAfterExpand();
				else //MOVETO_
					tname = trans[j]->getTransTarget();
				
				//2.2 target state��������
				NodeState* tstate = fsm->getState(tname);

				//2.3 target state�� ���� ���� newState�� ���Ե� transition����
				if(tstate != NULL){ //lsname�� g ���� ��� NULL���̴�.
					//std::string ls_new_prefix = sprefix + tstate->getEvtBhvStrInPrefix();
					//tstate->setPrefix(ls_new_prefix);
					
					//2.3.1 ���ο� state�� ���ϵ� trans ����
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
			
			//4. ���ο� state�� ��� ����
			newStates.insert(newState);

			//5. ���� state[i]�� trans ����
			states[i]->deleteAllTrans();

			//6. state[i]�� ���Ӱ� ����� trans
			NodeTrans* toS = new NodeTrans();
			toS->setTransTarget(targetType4toS, target4toS);
			toS->setTransSource(states[i]->getBhvName());
			toS->setStateAfterExpand(sname);
			states[i]->addTrans(toS);
		}
	}
	//7. ���Ӱ� ������ state�� fsm�� �߰�
	std::set<NodeState*>::iterator it = newStates.begin();
	for(;it!=newStates.end(); it++){
		fsm->addState(*it);
	}
	newStates.clear();
}

void TPLGenerator::addInitialState(FSM4BS* fsm)
{
	//fsm�� state���� prefix��� fsm�� prefix�� ���ؼ� start state�� �� �� �ִ�.
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
			//initial state�� trans����
			buildTrans(newInitState, (*it), (*it)->getEvtRef());
			//(*it)�� evtRef�� ���� trans�� ������ ����
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

	//fsm�� ���� �̸��� bhv�� initial state�� moveto�ϰ� ����
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
		//state���� �̸� ����
		NodeBhv* bhv = (NodeBhv*)bs;
		std::string bname = bhv->getBhvRef();
		std::string uname = getStateUName4ExpandBhv(bhv);
		if(!isSCname(bname) && getNumOfPrefix(bname, dupBhvs)>1)
			uname.append("_").append(bhv->getPrefix());
		bhv->setState(uname);
		//state���� �� �߰�
		NodeState* expand2con = new NodeState(bhv->getEvtRef(), bname, bhv->getPrefix());
		expand2con->setState(uname);
		
		//monitor�� �����ϴ� behavior�� expand�ϴ� state�� ��� ���� �Ҵ��
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
	//B[e:A->D->C] �� �� 
	else if(typeid(*bs)==typeid(NodeSeq)){
		NodeSeq* seq = (NodeSeq*)bs;
		std::vector<NodeBS*> cnodes = seq->getSubNodes();

		//1. seq�� ���Ե� state�� ����
		for(unsigned int i=0; i<cnodes.size(); i++){
			addStates4Bhvs(dupBhvs, fsm, seq, bsname, cnodes[i]);
		}

		//2. Transtion����(State�� ��� �����Ͽ��ٸ� �������� transition����)
		for(unsigned int j = 0; j<cnodes.size()-1; j++){
			NodeState* state1 = fsm->getState(cnodes[j]->getState());
			NodeState* state2 = fsm->getState(cnodes[j+1]->getState());

			
			std::string evtRef = cnodes[j]->getEvtRef();
			NodeTrans* newTrans = buildTrans(state1, state2, evtRef);

			//����ó�� ������ NodeBS�� ���� ���
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
������ state�� ����ó���� ������ �ִ� behavior�� ȣ���ϰ� �ִ� ���� üũ
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
	if(it != m_fsmTAB.end())//nest behavior�� ����
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
	//���Լ��� ȣ��� monPrefix�� ���Ǵ� �����̴�
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

		//�����尡 ������ ���� ��忡 ���� FSM�� �����Ѵ�.
		NodeBS* sub = bhv->getSubNode();
		std::string monOp  = bhv->getSubMonOp(); //""������ ���� ����
		std::string monEvt = bhv->getSubMonEvt();

		FSM4BS* fsm = NULL;
		if(sub != NULL){
			std::string prefix = sub->getPrefix();
			std::map<std::string, FSM4BS*>::iterator it = m_fsmTAB.find(prefix);
			if(it != m_fsmTAB.end()){
				//FSM�� �ټ��� BhvSeq�� ���� �� �ִ�.
				fsm = it->second;
				fsm->addBhvSeq(sub);
			}
			else{
				//FSM�� ������ �� 3���� �Լ��� ����
				std::string fsmName = refBhv;
				fsmName.append("_CON");
				fsm = new FSM4BS(prefix, refBhv, fsmName, type);
				if(monOp != "") fsm->setPrefix4MonTarget(bhv->getPrefix());
				fsm->addBhvSeq(sub);
				m_fsmTAB.insert(m_fsmTABPair(prefix, fsm));
			}

			//fsm�� ~, ^, *�� ����
			fsm->setMonOp(monOp);
			fsm->setMonEvt(monEvt);
			addEvtNode4Monitor(bhv);
			makeFSM(sub, type);
		}
	}
}

void TPLGenerator::makeFSM4ExptMon()
{
	//1. ������ fsm����� �̾Ƴ���.
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

	//2. ~ ���� ����ó�� event�� �Է¹޾� �̸� ó���ϴ� fsm�� �����ϴ� fsm����
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
			//state s0�� transition�� ����
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
			//~, ^, *�� ���� ������ΰ��
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
	if(it != myset.end()){//������
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