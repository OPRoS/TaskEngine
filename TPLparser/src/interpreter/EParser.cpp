/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "interpreter/EParser.h"
#include "ds/EStack.h"


EParser::EParser(std::ostream* ostr)
{
	BID = 0;
	SNodeID = 0;
	m_parsedResult = new GparsedResult();	
	m_incl = new Ginclude();
	log = Log("PARSER", ostr);
	log.setLevel(Log::LOG_LEVEL_DEBUG, true);
	log.setLevel(Log::LOG_LEVEL_ERROR, true);
}

EParser:: ~EParser(void)
{
	if(m_parsedResult != NULL) 
		delete m_parsedResult;

	if(m_incl != NULL) 
		delete m_incl;

	m_parsedResult = NULL;
	m_incl = NULL;
/*
	if(m_incl != NULL){
		while(m_incl->moveHead()){
			m_incl->delHead();
		}
		delete m_incl;
		m_incl = NULL;
	}
	*/
}


void EParser::navigator(pANTLR3_BASE_TREE tree, int indent)
{

	if(tree != NULL){
		for(int i=0; i<indent; i++){
			printf(" ");
		}
		//printf("Current Tree: %d and %s\n",tree->getChildCount(tree), tree->getText(tree)->chars);
		LOG_DEBUG(log, "Current Tree: %d and %s\n",tree->getChildCount(tree), tree->getText(tree)->chars);
	}

	unsigned int count = tree->getChildCount(tree);

	if(count > 0){
		for(unsigned int i=0; i<count;i++){
			pANTLR3_BASE_TREE c = (pANTLR3_BASE_TREE) (tree->getChild(tree, i));
			navigator(c, indent+1);
		}
	}
}

bool EParser::isStepping(pANTLR3_BASE_TREE tree)
{
	//stepping인 경우에는
	//EXPR expr_cond (asmtOp expr_add)?에서 (asmtOp expr_add)?가 오지 않는다.
	if(tree->getChildCount(tree) ==1)
	{
		unsigned int ruletype = tree->getType(tree);

		switch(ruletype){
			case EXPRPAR:
				return true;
			case ASMT:
				return true;
			case COND:
				return true;
			case BOOL:
				return true;
			case OR:
				return true;
			case AND:
				return true;
			case NOT:
				return true;
			case ADD:
				return true;
			case MUL:
				return true;
			case SIGN:
				return true;
			default:
				return false;
		}
	}
	return false;
}

std::string EParser::symTree3str(pANTLR3_BASE_TREE prim)
{
	char *tStr = (char*)prim->toStringTree(prim)->chars;

	//(ID a.c) 또는 (ID 4.3)을 a.c 과4.3으로 추출
	std::string s(tStr);
	//std::string p = std::string("ID");
	//std::string r("");
	Estring::replaceAll(s,"ID","");
	//p=std::string("(");
	Estring::replaceAll(s,"(","");
	//p=std::string(")");
	Estring::replaceAll(s,")","");
	std::string ts = Estring::trim(s);
	Estring::replaceAll(ts, " ", "");

	return ts;
}

bool EParser::isNOTsym(pANTLR3_BASE_TREE tree)
{
	//!(NOT)기호
	char *str = (char*)(tree->toString(tree)->chars);
	std::string sym(str);
	if(sym.compare("!")==0){
		return true;
	}

	return false;
}

std::string	EParser::getCurBlockFullName()
{
	//Behavior: workingnode(bhv1) + workingstate(s2) + workingblock(construct/destruct/entry/exit/stay/transition) + blockstmt
	//workingstate생략가능 ex)bhv1.construct
	//Connector: workingnode(c0) + workdingstate("") + workingblock(construct/destruct/conexer/sequexer) + blockstmt(with)
	

	std::string blockfullname = m_curWorkingNode;				
	if(m_curWorkingState != "") blockfullname.append(".").append(m_curWorkingState); 
	if(m_curWorkingBlock != "") blockfullname.append(".").append(m_curWorkingBlock); 
	if(m_curWorkingBlockStmt != "") blockfullname.append(".").append(m_curWorkingBlockStmt);
	
	return blockfullname;
}

int EParser::parseDef2(pANTLR3_BASE_TREE defTree)
{
	if(defTree == NULL){
		LOG_ERROR(log, "Parser tree could not be built. Check your Task syntax!");
		return PARSER_NO_TREE_PARSER;
	}

#ifdef __DEBUG_EParser_
	//LOG_TRACE(log, "Data parsed by ANTLR..."); // TRACE레벨이 활성화 안되면 안찍힐 것이다.
	printf((const char*)defTree->toStringTree(defTree)->chars);
	printf("\n");
#endif

	unsigned int cNum = defTree->getChildCount(defTree);

	if(cNum < 1){
		return PARSER_NO_TREE_PARSER;
	}
	else{

		//Ginclude* incl = NULL;

		for(unsigned int i=0; i<cNum; i++){

			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE)defTree->getChild(defTree,i);
			unsigned int eleType = child->getType(child);

			switch(eleType){
			case INCL:{
					//if(incl == NULL) incl = new Ginclude();
					//updateInclude(child, incl);
					break;
				}
			case BEHA:{
					int blockid = BID++;
					std::string blockpath = int2str(blockid);
					Gbehavior *bhv = new Gbehavior(m_curfName, -1, blockid, getCurBlockFullName(), blockpath);
					buildUpBehavior(blockpath, child, bhv);
					m_parsedResult->addbehavior(bhv);
					break;
				}
			case CNT:{
					int blockid = BID++;
					std::string blockpath = int2str(blockid);
					Gconnector *con = new Gconnector(m_curfName, -1, blockid, getCurBlockFullName(), blockpath);
					buildUpConnector(blockpath, child, con);
					m_parsedResult->addconnector(con);
					break;
			    }
			case TASK:{
					Gtask *task = getTask(child);
					m_parsedResult->addTask(task);
					break;
				}
			case ENUM:{
					Genum* gEnum = getEnum(child);
					m_parsedResult->addEnum(gEnum);
					break;
				}
			case MODEL:{
					Gmodel *gModel = getModel(child);
					m_parsedResult->addModel(gModel);
					break;
				}
			case ACTION:{
					Gaction *gAction = getAction(child);
					m_parsedResult->addAction(gAction);
					break;
				}
			//시나리오 EVTS, BHVS, BSS
			case EVTS:{
					buildUpSEvents(child);
					break;
				}
			case BHVS:{
					buildUpSBehaviors(child);
					break;
				}
			case BSS:{
					buildUpSC(child);
					break;
				}						
			case STASK:{
					buildUpSTask(child);
					break;
				}						
			default:
					break;
			}

		}
		//m_parsedResult->setInclude(incl);
		//task->setInclude(incl);
	}

	return 0;
}

//--------------------------------------------------------------------------------------------------
//                       시나리오
//--------------------------------------------------------------------------------------------------

void EParser::buildUpSEvents(pANTLR3_BASE_TREE events)
{
	if(events == NULL){
		LOG_ERROR(log, "Parser can't obtain the parser tree for <$Events>");
	}

	unsigned int cNum = events->getChildCount(events);
	if(cNum < 1){
	}
	else{
		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) events->getChild(events, i);
			unsigned int type = child->getType(child);
			if(type==EVT){
				SEvent* evt = getSEvent(child);
				m_parsedResult->addSEvent(evt);
			}
		}
	}
}

void EParser::buildUpSBehaviors(pANTLR3_BASE_TREE bhvs)
{
	if(bhvs == NULL){
		LOG_ERROR(log, "Parser can't obtain the parser tree for <$Behaviors>");
	}

	unsigned int cNum = bhvs->getChildCount(bhvs);
	if(cNum < 1){
	}
	else{
		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) bhvs->getChild(bhvs, i);
			unsigned int type = child->getType(child);
			if(type==BHV){
				SBehavior* bhv = getSBehavior(child);
				m_parsedResult->addSBehavior(bhv);
			}
		}
	}
}

void EParser::buildUpSTask(pANTLR3_BASE_TREE stask)
{
	if(stask == NULL){
		LOG_ERROR(log, "Parser can't obtain the parser tree for <$Scenario>");
	}
	
	unsigned int cNum = stask->getChildCount(stask);
	if(cNum < 1){
	}
	else{		
		STask* st = new STask();
		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) stask->getChild(stask, i);
			unsigned int type = child->getType(child);
			if(type==UID){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				std::string stname = (char*)n->getText(n)->chars;
				st->setName(stname);
			}
			if(type==START){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				std::string subtask = (char*)n->getText(n)->chars;
				st->setStart(subtask);
			}
		}
		if(st != NULL) m_parsedResult->addSTask(st);
	}
}

void EParser::buildUpSC(pANTLR3_BASE_TREE bss)
{
	if(bss == NULL){
		LOG_ERROR(log, "Parser can't obtain the parser tree for <$Scenario>");
	}
	
	unsigned int cNum = bss->getChildCount(bss);
	if(cNum < 1){
	}
	else{		
		SC* sc = new SC();
		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) bss->getChild(bss, i);
			unsigned int type = child->getType(child);
			if(type==UID){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				std::string scname = (char*)n->getText(n)->chars;
				sc->setName(scname);
			}
			if(type==BS){
				SBhvSeq* bs = getSBhvSeq(child);
				NodeBS* snode = bs->getStartNode();
				sc->addBS(bs);	
			}
			if(type==BS){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				std::string tystr = (char*)n->getText(n)->chars;
				sc->setType(tystr);
			}
		}
		if(sc != NULL) m_parsedResult->addSC(sc);
	}
}


SEvent*	EParser::getSEvent(pANTLR3_BASE_TREE evtTree)
{
	SEvent* sevt = NULL;
	if(evtTree == NULL){
		LOG_ERROR(log, "Parser can't obtain the parser tree for <$Scenario>");
	}

	unsigned int cNum = evtTree->getChildCount(evtTree);
	if(cNum < 1){
	}
	else{
		sevt = new SEvent();
		std::string nameStr;
		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) evtTree->getChild(evtTree, i);
			unsigned int type = child->getType(child);
			if(type==UID){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				nameStr = (char*)n->getText(n)->chars;
				Gtoken* name = new Gtoken(m_curfName, "", "", nameStr, n->getLine(n), n->getType(n));
				sevt->setName(name);
			}
			if(type==EVTEXPR){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				parseStrBlock((char*)n->getText(n)->chars, EVTEXPR, sevt, nameStr);
			}
			if(type==DESC){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				Gtoken* desc = new Gtoken(m_curfName, "", "", (char*)n->getText(n)->chars, n->getLine(n), n->getType(n));			
				sevt->setDesc(desc);
			}
		}
		
	}
	return sevt;
}

SBehavior* EParser::getSBehavior(pANTLR3_BASE_TREE bhvTree)
{
	SBehavior* sbhv = NULL;
	if(bhvTree == NULL){
		LOG_ERROR(log, "Parser can't obtain the parser tree for <$Scenario>");
	}

	unsigned int cNum = bhvTree->getChildCount(bhvTree);
	if(cNum < 1){
	}
	else{
		sbhv = new SBehavior();
		std::string nameStr;
		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) bhvTree->getChild(bhvTree, i);
			unsigned int type = child->getType(child);
			if(type==UID){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				nameStr = (char*)n->getText(n)->chars;
				Gtoken* name = new Gtoken(m_curfName, "", "", nameStr, n->getLine(n), n->getType(n));
				sbhv->setName(name);
			}
			if(type==STRBLOCK){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				parseStrBlock((char*)n->getText(n)->chars, STRBLOCK, sbhv, nameStr);
				
			}
			if(type==STPBLOCK){
			}
			if(type==RSMBLOCK){
			}
			if(type==TYPE){
			}
			if(type==MODE){
			}
			if(type==DESC){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				Gtoken* desc = new Gtoken(m_curfName, "", "", (char*)n->getText(n)->chars, n->getLine(n), n->getType(n));			
				sbhv->setDesc(desc);
			}
		}
	}
	return sbhv;
}

SBhvSeq* EParser::getSBhvSeq(pANTLR3_BASE_TREE bsTree)
{
	SBhvSeq * sbs = NULL;
	if(bsTree == NULL){
		LOG_ERROR(log, "Parser can't obtain the parser tree for <$Scenario>");
	}

	unsigned int cNum = bsTree->getChildCount(bsTree);
	if(cNum < 1){
	}
	else{
		sbs = new SBhvSeq();
		NodeBS* startnode = NULL;
		std::string nameStr;
		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) bsTree->getChild(bsTree, i);
			unsigned int type = child->getType(child);
			if(type==UID){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				nameStr = (char*)n->getText(n)->chars;
				Gtoken* name = new Gtoken(m_curfName, "", "", nameStr, n->getLine(n), n->getType(n));
				sbs->setName(name);
			}
			if(type==BSEQ){
				startnode = getNodeSeq(child);	
				sbs->setStartNode(startnode);
			}
			if(type==GOT){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				std::string gt = (char*)n->getText(n)->chars;
				sbs->setGoto(gt);
			}
			if(type==DESC){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				Gtoken* desc = new Gtoken(m_curfName, "", "", (char*)n->getText(n)->chars, n->getLine(n), n->getType(n));			
				sbs->setDesc(desc);
			}
		}
	}
	return sbs;
}

NodeBS* EParser::getNodeSeq(pANTLR3_BASE_TREE seqTree)
{
	NodeSeq* node = NULL;
	if(seqTree == NULL){
		LOG_ERROR(log, "Parser can't obtain the parser tree for <$Scenario>");
	}

	//printf((const char*)seqTree->toStringTree(seqTree)->chars);

	unsigned int cNum = seqTree->getChildCount(seqTree);
	if(cNum < 1){
	}
	else{	
		//if(cNum>1) 
			node = new NodeSeq(SNodeID++);
		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) seqTree->getChild(seqTree, i);
			unsigned int type = child->getType(child);
			if(type==SEQ_ELEM){
				NodeBS* bs = getNodeBS(child);
				//if(cNum > 1) 
					node->addSub(bs);
				//else {
				//	delete node;
				//	return bs;
				//}
			}
		}
	}
	return node;
}

NodeBS* EParser::getNodeCon(pANTLR3_BASE_TREE con)
{
	NodeBS* node = NULL;
	if(con == NULL){
		LOG_ERROR(log, "Parser can't obtain the parser tree for <$Scenario>");
	}

	unsigned int cNum = con->getChildCount(con);
	if(cNum < 1){
	}
	else{	
		if(cNum>1) node = new NodeCon(SNodeID++);
		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) con->getChild(con, i);
			unsigned int type = child->getType(child);
			if(type==JTYPE){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				unsigned int ty = getJoinType((char*)n->getText(n)->chars);
				((NodeCon*)node)->setJoinType(ty);
			}
			if(type==BSEQ){
				NodeBS* seq = getNodeSeq(child);
				if(cNum > 1) 
					((NodeCon*)node)->addSub(seq);
				else {
					delete node;
					return seq;
				}
			}
		}		
	}
	return node;
}

NodeBS* EParser::getNodeBS(pANTLR3_BASE_TREE bs)
{
	std::string evt_ref = "";

	if(bs == NULL){
		LOG_ERROR(log, "Parser can't obtain the parser tree for <$Scenario>");
	}

	//printf((const char*)bs->toStringTree(bs)->chars);

	unsigned int cNum = bs->getChildCount(bs);
	if(cNum < 1){
	}
	else{	
		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) bs->getChild(bs, i);
			unsigned int type = child->getType(child);
			if(type==EVT_ID){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				//id
				evt_ref = (char*)n->getText(n)->chars;
			}
			if(type==BHV_ID){
				NodeBhv* nodebhv = new NodeBhv(SNodeID++);
				nodebhv->setEvtRef(evt_ref);
				//id
				pANTLR3_BASE_TREE id = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				nodebhv->setBhvRef((char*)id->getText(id)->chars);
				//내포된 [bhv_seq]
				pANTLR3_BASE_TREE mon = (pANTLR3_BASE_TREE) child->getChild(child, 1);
				if(mon != NULL){ 
					//monitor 종류
					pANTLR3_BASE_TREE monchild0 = (pANTLR3_BASE_TREE)mon->getChild(mon, 0);
					pANTLR3_BASE_TREE opchild = (pANTLR3_BASE_TREE)monchild0->getChild(monchild0, 0);
					std::string monOp = (char*)opchild->getText(opchild)->chars;
					nodebhv->setSubMonOp(monOp);
					//monitiro 트리거 이벤트
					pANTLR3_BASE_TREE monchild1 = (pANTLR3_BASE_TREE)mon->getChild(mon, 1);
					pANTLR3_BASE_TREE evtchild = (pANTLR3_BASE_TREE)monchild1->getChild(monchild1, 0);
					std::string monEvt = (char*)evtchild->getText(evtchild)->chars;
					nodebhv->setSubMonEvt(monEvt);
					//모니터 sequence
					pANTLR3_BASE_TREE monchild2 = (pANTLR3_BASE_TREE)mon->getChild(mon, 2);
					NodeBS* ns = getNodeSeq(monchild2);
					nodebhv->setSub(ns);
				}
				return nodebhv;
			}
			if(type==CON){
				NodeBS* nodecon = getNodeCon(child);
				nodecon->setEvtRef(evt_ref);
				return nodecon;
			}
		}
	}
	return NULL;;
}

//--------------------------------------------------------------------------------------------------

void EParser::buildUpBehavior(const std::string blockpath, pANTLR3_BASE_TREE taskTree, Gbehavior *gBhv)
{
	if(taskTree == NULL){
		LOG_ERROR(log, "Parser can't obtain the parser tree for TASK syntax");
	}

	unsigned int cNum = taskTree->getChildCount(taskTree);

	if(cNum < 1){
	}
	else{

		//Gbehavior *gBhv = new Gbehavior();
		for(unsigned int i=0; i<cNum; i++){

			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) taskTree->getChild(taskTree, i);
			unsigned int type = child->getType(child);

			if(type==TNAME){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				Gtoken *tName = new Gtoken(m_curfName, getCurBlockFullName(), blockpath, (char*)n->getText(n)->chars, n->getLine(n), n->getType(n));				
				gBhv->setName(tName);
				m_curWorkingNode = std::string((char*)n->getText(n)->chars);
			}
			else if(type==PARMS){
				EList<Gparameter*>* params = getParameters(blockpath, child);
				gBhv->setParameters(params);
			}
			else if(type == EOL){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				unsigned int line = n->getLine(n);
				gBhv->setBlockName(getCurBlockFullName());
				gBhv->setEndLine(line);
			}
			else if(type==BHVELEM){				
				EList<Gstate*>* states = getStates(blockpath, child);				
				GstmtBlock* cons = getConstruct(blockpath, child);
				GstmtBlock* dest = getDestruct(blockpath, child);
				gBhv->setStates(states);
				gBhv->setConstruct(cons);
				gBhv->setDestruct(dest);
			}
			else if(type==VB){
				//pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				buildUpVarBlock(blockpath, child, gBhv);
			}
			else if(type==MEM){
				gBhv->setAsMission();
			}
			/*
			if(type == LVAR){
				GlocalVar *stmt = getLocalVarDecl(blockpath, child);
				//block지정
				std::string blockfullname = m_curWorkingNode;				
				if(m_curWorkingBlock != "") blockfullname.append(".").append("var"); 
				stmt->setBlockFullName(blockfullname);				
				gBhv->addCstVar(stmt);
			}
			*/
		}
	}

	m_curWorkingNode = "";
}

void EParser::buildUpConnector(const std::string blockpath, pANTLR3_BASE_TREE conTree, Gconnector *gcon)
{
	if(conTree == NULL){
		LOG_ERROR(log, "Parser can't obtain the parser tree for TASK syntax");
	}

	unsigned int cNum = conTree->getChildCount(conTree);

	if(cNum < 1){
	}
	else{
		//gcon->setLine(conTree->getLine(conTree));

		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) conTree->getChild(conTree, i);
			unsigned int type = child->getType(child);

			if(type==NAME){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				Gtoken *tName = new Gtoken(m_curfName, getCurBlockFullName(), blockpath, (char*)n->getText(n)->chars, n->getLine(n), n->getType(n));			
				gcon->setName(tName);
				m_curWorkingNode = std::string((char*)n->getText(n)->chars);
			}
			else if(type==PARMS){
				EList<Gparameter*>* params = getParameters(blockpath, child);
				gcon->setParameters(params);
			}
			else if(type==VB){
				//pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				buildUpVarBlock(blockpath, child, gcon);
			}
			else if(type==ELEMS){//Behavior와 다르게 내부에서 처리하도록 하는 방법 사용
				unsigned int cNum2 = child->getChildCount(child);
				if(cNum2 >= 1){
					for(unsigned int j=0; j<cNum2; j++){
						pANTLR3_BASE_TREE child2 = (pANTLR3_BASE_TREE) child->getChild(child, j);
						int type = child2->getType(child2);
						if(type == CON){
							//std::string cbid = BID++;
							//std::string chBlockpath = blockpath;							
							//chBlockpath.append(".").append(cbid); // statement개념이기 때문에 굳이 blockpath를 늘릴 필요가 없다.
							buildUpConBlock(blockpath, child2, gcon);
						}
						else if(type == CONSTR){
							GstmtBlock* cons = getActionBlock(blockpath, child2);
							gcon->setConstruct(cons);
						}
						else if(type == DEST){
							GstmtBlock* dest = getActionBlock(blockpath, child2);
							gcon->setDestruct(dest);
						}
					}
				}
			}
			else if(type==EOL){//connector의 마지막 }			
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				unsigned int line = n->getLine(n);
				gcon->setEndLine(line); //GstmtBlock 생성시의 -1이 여기서 올바르게 세팅
			}
			else if(type==MEM){
				gcon->setAsMission();
			}
		}
	}
	m_curWorkingNode = "";
}

/**
conexer, seqexer
*/
void EParser::buildUpConBlock(const std::string blockpath, pANTLR3_BASE_TREE constmtTree, Gconnector* gcon)
{
	unsigned int cNum = constmtTree->getChildCount(constmtTree);

	if(cNum < 1){
	}
	else{
		//m_curWorkingBlock = "conexer";
		GstmtBlock* withs = new GstmtBlock(m_curfName, -1, BID++, getCurBlockFullName(), blockpath);
		

		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) constmtTree->getChild(constmtTree, i);
			unsigned int type = child->getType(child);

			if(type==SYNMOD){//'synch' 'asynch'
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				unsigned int ty = getSynchType((char*)n->getText(n)->chars);
				gcon->setRunType((int)ty);
			}
			else if(type==CMOD){//'conexer' 'seqexer'
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				unsigned int ty = getConnType((char*)n->getText(n)->chars);
				gcon->setConnType((int)ty);
				m_curWorkingBlock = std::string((char*)n->getText(n)->chars);
				withs->setBlockName(getCurBlockFullName());		
			}
			else if(type==JTYPE){//'and-join' 'or-join'
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				unsigned int ty = getJoinType((char*)n->getText(n)->chars);
				gcon->setJoinType((int)ty);
				//EOL로 사용
				unsigned int line = n->getLine(n);
				withs->setEndLine(line); 
			}
			
			else if(type==WITHS){

				std::string gid = gcon->getBlockID();
				std::string path = blockpath;
				path.append(".").append(gid);
			
				unsigned int cNum2 = child->getChildCount(child);
				for(unsigned int n=0; n<cNum2; n++){
					pANTLR3_BASE_TREE child2 = (pANTLR3_BASE_TREE) child->getChild(child, n);
					unsigned int type = child->getType(child);
					GstmtBlock* runblock = getRunblock(path, child2);
					if(runblock != NULL){
						//if(runBlocks == NULL) runBlocks = new EList<Gstmt*>();
						withs->addStmt(runblock);
					}
				}
				gcon->setWiths(withs);
	
				//WITHS에 run블록이 여러개 있기 때문에 getStmts
			//	EList<Gstmt*>* stmts = getStmts(blockpath, child);
				//if(stmts != NULL){
				//	withs->setStmts(stmts);
			//		gcon->setWiths(withs);
			//	}
			}
			else if(type == EOL){			
				//';'기호가아니라 위의 JTYPE을 endline으로 사용
				//pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				//unsigned int line = n->getLine(n);
				//withs->setEndLine(line); //GstmtBlock 생성시의 -1이 여기서 올바르게 세팅
			}
		}
		m_curWorkingBlock="";
	}
}

void EParser::buildUpVarBlock(const std::string blockpath, pANTLR3_BASE_TREE varTree, GstmtBlock* block)
{
	unsigned int cNum = varTree->getChildCount(varTree);

	if(cNum < 1){
	}
	else{
		std::string blockID = block->getBlockID();
		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) varTree->getChild(varTree, i);
			unsigned int type = child->getType(child);
			unsigned int line = child->getLine(child);		
			if(type == LVAR){
				m_curWorkingBlock="var";
				GlocalVar *stmt = getLocalVarDecl(blockpath, child);									
				stmt->setBlockName(getCurBlockFullName());
				stmt->setEndLine(line);
				block->addStmt(stmt); // GcstBlock의setCstVars를 사용못한 것은 State(GstmtBlock* 타입)도 변수가 있기 때문
			}
		}
	}
	m_curWorkingBlock="";
}


EList<Gparameter*>* EParser::getParameters(const std::string blockpath, pANTLR3_BASE_TREE paramsTree)
{
	unsigned int cNum = paramsTree->getChildCount(paramsTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		EList<Gparameter*>* parameters = new EList<Gparameter*>();

		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) paramsTree->getChild(paramsTree, i);
			Gparameter *param = getParameter(blockpath, child);
			if(param != NULL){
				parameters->addTail(param);
			}
		}
		return parameters;
	}
}

Gparameter* EParser::getParameter(const std::string blockpath, pANTLR3_BASE_TREE paramTree)
{
	unsigned int cNum = paramTree->getChildCount(paramTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		Gparameter *parameter = new Gparameter();
		//parameter->setLine(paramTree->getLine(paramTree));

		unsigned int cNum = paramTree->getChildCount(paramTree);
		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) paramTree->getChild(paramTree, i);
			unsigned int type = child->getType(child);
			switch(type){
				case TYPE:{
					pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
					unsigned int type2 = n->getType(n);
					switch(type2){
						case PTYP:{
							pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE)n->getChild(n, 0);
							Gtoken *type = new Gtoken(m_curfName, getCurBlockFullName(), blockpath, (char*)n2->getText(n2)->chars, n2->getLine(n2), n2->getType(n2));
							parameter->setType(type);
							break;
						}
						case STYP:{
							pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE)n->getChild(n, 0);
							pANTLR3_BASE_TREE n3 = (pANTLR3_BASE_TREE)n2->getChild(n2, 0);
							Gtoken *type = new Gtoken(m_curfName, getCurBlockFullName(), blockpath, (char*)n3->getText(n3)->chars, n3->getLine(n3), n3->getType(n3));
							parameter->setType(type);
							break;
						}
						default:
							break;
					}
					//아래 break 매우 중요
					break;
				}
				case NAME:{
					pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
					pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE) n->getChild(n, 0);
					Gtoken *name = new Gtoken(m_curfName, getCurBlockFullName(), blockpath, (char*)n2->getText(n2)->chars, n2->getLine(n2), n2->getType(n2));
					parameter->setName(name);
					break;
				}
				default:
					break;
			}
		}
		return parameter;
	}
}


EList<Gstate*>* EParser::getStates(const std::string blockpath, pANTLR3_BASE_TREE statesTree)
{
	unsigned int cNum = statesTree->getChildCount(statesTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		EList<Gstate*> *states = new EList<Gstate*>();

		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) statesTree->getChild(statesTree, i);
			int type = child->getType(child);
		//	char *name = (char*)child->toString(child)->chars;
		//	std::string sName(name);

			if(type == STATE){
				Gstate* pState = getState(blockpath, child);
				states->addTail(pState);
			}
		}
		return states;
	}
}

GstmtBlock* EParser::getConstruct(const std::string blockpath, pANTLR3_BASE_TREE statesTree)
{
	unsigned int cNum = statesTree->getChildCount(statesTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		GstmtBlock* gstmtBlock = NULL;
		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) statesTree->getChild(statesTree, i);
			int type = child->getType(child);
			if(type == CONSTR){
				gstmtBlock = getActionBlock(blockpath, child);
			}
		}
		return gstmtBlock;
	}
}


GstmtBlock* EParser::getDestruct(const std::string blockpath, pANTLR3_BASE_TREE statesTree)
{
	unsigned int cNum = statesTree->getChildCount(statesTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		GstmtBlock* gstmtBlock = NULL;
		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) statesTree->getChild(statesTree, i);
			int type = child->getType(child);
			if(type == DEST){
				gstmtBlock = getActionBlock(blockpath, child);
			}
		}
		return gstmtBlock;
	}
}

Gstate* EParser::getState(const std::string blockpath, pANTLR3_BASE_TREE stateTree)
{
	unsigned int cNum = stateTree->getChildCount(stateTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		//Gstate* state = new Gstate(m_curfName, -1, BID++, getCurBlockFullName(), blockpath);
		Gstate* state = new Gstate(m_curfName, BID++);
		EList<Gstmt*>* stateVarList = NULL;

		//block ID
		std::string sid = state->getBlockID();
		std::string path = blockpath;
		path.append(".").append(sid);
		state->setBlockPath(path);
		
		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE)stateTree->getChild(stateTree, i);
			unsigned int type = child->getType(child);
			//char* temp = (char*)child->toStringTree(child)->chars;						

			if(type == SMOD){
				//printf("SMOD:%s\n",child->toStringTree(child)->chars);
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				int mod = getStateMod((char*)n->getText(n)->chars);
				state->setModifier(mod);
			}

			if(type == SNAME){
				//printf("TNAME:%s\n",child->toStringTree(child)->chars);
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);			
				m_curWorkingState = (char*)n->getText(n)->chars;				
				Gtoken *sName = new Gtoken(m_curfName, getCurBlockFullName(), blockpath, getCurBlockFullName().c_str(), n->getLine(n), n->getType(n));
				state->setName(sName);
				state->setBlockName(getCurBlockFullName());
			}

			if(type == EOL){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				unsigned int line = n->getLine(n);				
				state->setBlockName(getCurBlockFullName());	
				state->setEndLine(line);
			}

			if(type == ACTION){
				//printf("ACTION:%s\n",child->toStringTree(child)->chars);
				GstmtBlock *block = getActionBlock(path, child);		
				state->setActionBlock(block);
			}

			if(type == TRANS){
				//printf("TRANS:%s\n",child->toStringTree(child)->chars);
				GstateTrans *trans = getTransition(path, child);							
				state->setTransition(trans);
			}
/*
			if(type == LVAR){ //(local variable) 이게 여러개 올 수 있다. var:다음에 오는 각각의 state변수들
				m_curWorkingBlock="var";
				GlocalVar *stmt = getLocalVarDecl(path, child);
				//block지정
				std::string blockfullname = m_curWorkingNode;				
				if(m_curWorkingState != "") blockfullname.append(".").append(m_curWorkingState); 
				if(m_curWorkingBlock != "") blockfullname.append(".").append(m_curWorkingBlock); 
				stmt->setBlockFullName(blockfullname);		
				//if(stmt != NULL)
				//	if(stateVarList == NULL) stateVarList = new EList<Gstmt*>();
				//stateVarList->addTail(stmt);
				state->addStateVar(stmt);
			}
*/
			if(type==VB){
				//pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				buildUpVarBlock(path, child, state);
			}
		}
		m_curWorkingState = "";
		return state;
	}
}

GstateTrans* EParser::getTransition(const std::string blockpath, pANTLR3_BASE_TREE tranTree)
{
	/*
	unsigned int cNum = tranTree->getChildCount(tranTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		GstateTrans *trans = new GstateTrans(BID++);
		std::string trid = trans->getBlockID();
		std::string path = blockpath;
		path.append(".").append(trid);

	
		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE)tranTree->getChild(tranTree, i);
			unsigned int type = child->getType(child);			
			if(type == IF){
				GstmtIf *ifstmt = getIfstmt(path, child);
				trans->addIfstmt(ifstmt);			
			}
		}


		return trans;
	}
	*/

	unsigned int cNum = tranTree->getChildCount(tranTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		GstateTrans* trans = new GstateTrans(m_curfName, -1, BID++, "", blockpath);
		m_curWorkingBlock="transition";
		trans->setBlockName(getCurBlockFullName());			

		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE)tranTree->getChild(tranTree, i);
			unsigned int type = child->getType(child);		
			
			if(type == STMTS){		
				//block ID
				std::string trid = trans->getBlockID();
				std::string path = blockpath;
				path.append(".").append(trid);
				EList<Gstmt*>* stmts = getStmts(path, child);
				if(stmts != NULL){
					trans->setStmts(stmts);
				}
			}

			if(type == EOL){				
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
				unsigned int line = n->getLine(n);
				trans->setEndLine(line);				
			}
		}

		m_curWorkingBlock="";
		return trans;
	}
}

GstmtBlock* EParser::getActionBlock(const std::string blockpath, pANTLR3_BASE_TREE actionTree)
{
	unsigned int cNum = actionTree->getChildCount(actionTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		//GstmtBlock *block = new GstmtBlock(BID++);
		GstmtBlock *block = NULL;
		std::string blockid = "";
		std::string path = blockpath;

		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE)actionTree->getChild(actionTree, i);
			unsigned int type = child->getType(child);

			if(type==AMOD){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				std::string mod((char*)n->getText(n)->chars);

				if(mod.compare("entry")==0){
					m_curWorkingBlock="entry";
					block = new GstateEntry(m_curfName, -1, BID++, getCurBlockFullName(), blockpath);
				}
				else if(mod.compare("stay")==0){
					m_curWorkingBlock="stay";
					block = new GstateDo(m_curfName, -1, BID++, getCurBlockFullName(), blockpath);
				}
				else if(mod.compare("exit")==0){
					m_curWorkingBlock="exit";
					block = new GstateExit(m_curfName, -1, BID++, getCurBlockFullName(), blockpath);
				}
				else if(mod.compare("construct")==0){
					m_curWorkingBlock="construct";
					block = new GstmtBlock(m_curfName, -1, BID++, getCurBlockFullName(), blockpath);
				}
				else if(mod.compare("destruct")==0){
					m_curWorkingBlock="destruct";
					block = new GstmtBlock(m_curfName, -1, BID++, getCurBlockFullName(), blockpath);
				}
				else if(mod.compare("initializer")==0){
					m_curWorkingBlock="initializer";
					block = new GstmtBlock(m_curfName, -1, BID++, getCurBlockFullName(), blockpath);
				}
				else if(mod.compare("finalizer")==0){
					m_curWorkingBlock="finalizer";
					block = new GstmtBlock(m_curfName, -1, BID++, getCurBlockFullName(), blockpath);
				}
				else if(mod.compare("run")==0){
					m_curWorkingBlock="run";
					block = new GstmtBlock(m_curfName, -1, BID++, getCurBlockFullName(), blockpath);
				}
				else{
					block = NULL; //에러메시지 출력해야하는데..(점검요망) stay 대신 do를 사용해 보라..
				}

				if(block != NULL){
					//block ID
					blockid = block->getBlockID();
					path.append(".").append(blockid);
					//block NAME					
					block->setBlockName(getCurBlockFullName());		
				}
			}

			//STMTS 다음에 SEQ, PAR, STMTS등이 서브로 온다
			if(type == STMTS){
				//pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				EList<Gstmt*>* stmts = getStmts(path, child);
				if(stmts != NULL && block != NULL){
					block->setStmts(stmts);
				}
			}
			if(type == PERIOD){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
				//Gtoken *vname = new Gtoken(m_curfName, getCurBlockFullName(), blockpath, (char*)n->getText(n)->chars, n->getLine(n), n->getType(n));
				std::string str = (char*)n->getText(n)->chars;
				int period = str2int(str);
				setPeriod(block, period);
			}
			if(type == EOL){				
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				unsigned int line = n->getLine(n);
				block->setEndLine(line); //GstmtBlock 생성시의 -1이 여기서 올바르게 세팅
			}
		}
		m_curWorkingBlock="";			
				
		return block;
	}
}

/**
EOL에 처리가 있는 것도 있고 없는 것도 있는데 이는 문법 구조상 ';' 나 '}'가 정의문의 밖에 따로 있기 때문
규칙의 재사용성 때문에 어쩔 수 없다.
*/
EList<Gstmt*>* EParser::getStmts(const std::string blockpath, pANTLR3_BASE_TREE stmtsTree)
{
	unsigned int cNum = stmtsTree->getChildCount(stmtsTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		//GstmtBlock *stmts = new GstmtBlock(BID++);
		EList<Gstmt*>* stmtlist = new EList<Gstmt*>();

		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE)stmtsTree->getChild(stmtsTree, i);
			Gstmt* stmt = getStmt(blockpath, child);
				
			if(stmt != NULL){
				stmtlist->addTail(stmt);
			}
			else{
				//에러코드 : 대부분 문법을 바꾸면서 실수 하는 경우
			}
		}
		return stmtlist;
	}
}

GstmtBlock* EParser::getRunblock(const std::string blockpath, pANTLR3_BASE_TREE stmtsTree)
{
	GstmtBlock* stmt = NULL;

	m_curWorkingBlockStmt = "run";
	std::string chBlockpath = blockpath;							
	chBlockpath.append(".").append(int2str(BID++));

	unsigned int cNum = stmtsTree->getChildCount(stmtsTree);
	
	if(cNum < 1) return NULL;

	for(unsigned int i=0; i<cNum; i++){
		pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE)stmtsTree->getChild(stmtsTree, i);
		int type = child->getType(child);
		if(type == STMTS){
			//child가 아니라 stmtsTree를 인자로 넘긴다
			stmt = getStmtBlock(chBlockpath, stmtsTree);
		}
		if(type == PERIOD){
			pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
			//Gtoken *vname = new Gtoken(m_curfName, getCurBlockFullName(), blockpath, (char*)n->getText(n)->chars, n->getLine(n), n->getType(n));
			std::string str = (char*)n->getText(n)->chars;
			int period = str2int(str);
			setPeriod(stmt, period);
		}
		if(type==AMOD){
		}
		if(type==EOL){
		}
	}
	return stmt;
}

Gstmt* EParser::getStmt(const std::string blockpath, pANTLR3_BASE_TREE stmtsTree)
{
	int type = stmtsTree->getType(stmtsTree);
	Gstmt* stmt = NULL;

	if(type == IF){
		stmt = getIfstmt(blockpath, stmtsTree);
	}
	else if(type == GOTO){
		stmt = getGotoState(blockpath, stmtsTree);
	}
	else if(type == IVK){
		stmt = getInvokeSub(blockpath, stmtsTree);
	}

	//else if(type == CALL){
	//	stmt = getFuncall(blockpath, child);
	//}
	else if(type == STMTCALL){

		stmt = getCallStmt(blockpath, stmtsTree);
		/*
		pANTLR3_BASE_TREE child2 = (pANTLR3_BASE_TREE)child->getChild(child, 0);
		stmt = getCallStmt(blockpath, child2);
		//2. 라인넘버 지정
		pANTLR3_BASE_TREE child3 = (pANTLR3_BASE_TREE)child->getChild(child, 1);
		unsigned int line = child3->getLine(child3);
		stmt->setEndLine(line);		
		*/
	}
	//else if(type == ASMT){
	//	stmt = getExpr(blockpath, child);
	//}
	else if(type == STMTEXPR){
		//1.expr 객체 지정
		pANTLR3_BASE_TREE child2 = (pANTLR3_BASE_TREE)stmtsTree->getChild(stmtsTree, 0);
		stmt = getExpr(blockpath, child2);
		//2. 라인넘버 지정
		pANTLR3_BASE_TREE child3 = (pANTLR3_BASE_TREE)stmtsTree->getChild(stmtsTree, 1);
		unsigned int line = child3->getLine(child3);
		stmt->setEndLine(line);							
		//block명 지정
		/*std::string blockfullname = m_curWorkingBhv;
		if(m_curWorkingState != "") blockfullname.append(".").append(m_curWorkingState); 
		if(m_curWorkingBlock != "") blockfullname.append(".").append(m_curWorkingBlock); 
		stmt->setBlockFullName(blockfullname);*/
	}		
	else if(type == LVAR){
		stmt = getLocalVarDecl(blockpath, stmtsTree);
	}					
	else if(type == WAIT){
		stmt = getWait(blockpath, stmtsTree);
	}
	else if(type == SYNCH){
		stmt = getSynch(blockpath, stmtsTree);
	}
	
	//else if(type == WITH){
	else if(type == RUN){
		m_curWorkingBlockStmt = "with";
		std::string chBlockpath0 = blockpath;							
		chBlockpath0.append(".").append(int2str(BID++));



		unsigned int cNum = stmtsTree->getChildCount(stmtsTree);
		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE)stmtsTree->getChild(stmtsTree, i);
			int type = child->getType(child);
			if(type == STMTS){
/*				
				std::string chBlockpath = blockpath;							
				chBlockpath.append(".").append(int2str(BID++));

				EList<Gstmt*>* subStmts= getStmts(chBlockpath, child);
				

				//with를 stmt로 보면 끝이 없기 때문에 디버깅시에 empty메시가 나온다. 이걸 없애려고
				//어차피 with는 하나의 stmt만 가지기 때문에
				if(subStmts!=NULL && subStmts->getSize()>0) stmt = subStmts->getCurObject();

				//with문 껍데기 삭제
				if(subStmts != NULL){
					while(subStmts->moveHead()){
						//Gstmt* s = subStmts->getCurObject();
						//delete s; //알멩이는 위에서 사용 지우면 안된다.
						subStmts->delHead();
					}
					delete subStmts;
				}
*/
				GstmtBlock* sblock = getStmtBlock(chBlockpath0, stmtsTree);
				stmt = (Gstmt*)sblock;
			}
			if(type == PERIOD){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
				//Gtoken *vname = new Gtoken(m_curfName, getCurBlockFullName(), blockpath, (char*)n->getText(n)->chars, n->getLine(n), n->getType(n));
				std::string str = (char*)n->getText(n)->chars;
				int period = str2int(str);
				setPeriod(stmt, period);
			}
		}
	}

	else if(type == STMTBLOCK || type == SEQ|| type == PAR){
		//GstmtBlock* sblock = new GstmtBlock(m_curfName, -1, BID++, getCurBlockFullName(), blockpath);
		GstmtBlock* sblock = getStmtBlock(blockpath, stmtsTree);
		if(type == STMTBLOCK){
			/*
			sblock->setModifier(STMTBLOCK);
			//EOL설정
			pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
		    unsigned int line = n->getLine(n);
		    sblock->setEndLine(line);
			*/					
			sblock->setModifier(STMTBLOCK);
		}
		if(type == SEQ){
			m_curWorkingBlockStmt = "sequential";
			sblock->setModifier(SEQ);
		}
		if(type == PAR){ 
			m_curWorkingBlockStmt = "parellel";
			sblock->setModifier(PAR);
		}
		/*
		if(type == WITH) {				
			m_curWorkingBlock.append("with");
			sblock->setModifier(WITH);
		}
		
		std::string sbid = sblock->getBlockID();
		std::string chBlockpath = blockpath;								
		chBlockpath.append(".").append(sbid);

		EList<Gstmt*>* subStmts= getStmts(chBlockpath, child);
		sblock->setStmts(subStmts);
		*/
		stmt = (Gstmt*)sblock;
	}
		
	if(stmt != NULL){			
		stmt->setBlockName(getCurBlockFullName());
	}
	else{
		//에러코드 : 대부분 문법을 바꾸면서 실수 하는 경우
	}

	m_curWorkingBlockStmt = "";
	return stmt;
}

void EParser::setPeriod(Gstmt* stmt, int period)
{
	if(stmt == NULL) return;

	if(stmt->getStmtName() == STMT_CallBhv){
		((GstmtInvokeSub*)stmt)->setPeriod(period);
		return;
	}
	else if(stmt->getStmtName() == STMT_Block){
		EList<Gstmt*>* stmtlist = ((GstmtBlock*)stmt)->getStmts();
		if(stmtlist != NULL){
			stmtlist->moveHead();
			do{
				Gstmt *stmt = stmtlist->getCurObject();
				setPeriod(stmt, period);
			}
			while(stmtlist->moveNext());
		}
	}
	else if(stmt->getStmtName() == STMT_if){
		Gstmt* ifdo = ((GstmtIf*)stmt)->getIfAction();
		setPeriod(ifdo, period);
		Gstmt* elsedo = ((GstmtIf*)stmt)->getElse();
		setPeriod(elsedo, period);
	}
}

GstmtBlock* EParser::getStmtBlock(const std::string blockpath, pANTLR3_BASE_TREE stmtsTree)
{
	unsigned int cNum = stmtsTree->getChildCount(stmtsTree);
	if(cNum < 1){
		return NULL;
	}
	else{
		GstmtBlock* sblock = new GstmtBlock(m_curfName, -1, BID++, getCurBlockFullName(), blockpath);

		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE)stmtsTree->getChild(stmtsTree, i);
			int type = child->getType(child);
			if(type==STMTS){
				std::string sbid = sblock->getBlockID();
				std::string chBlockpath = blockpath;								
				chBlockpath.append(".").append(sbid);

				EList<Gstmt*>* subStmts= getStmts(chBlockpath, child);
				sblock->setStmts(subStmts);				
			}
			else if(type==EOL){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
				unsigned int line = n->getLine(n);
				sblock->setEndLine(line);
			}
		}
		return sblock;
	}
}

GlocalVar* EParser::getLocalVarDecl(const std::string blockpath, pANTLR3_BASE_TREE varTree)
{
	unsigned int cNum = varTree->getChildCount(varTree);
	if(cNum < 1){
		return NULL;
	}
	else{
#ifdef __DEBUG_EParser_
//파싱결과가 궁금할 때...
printf("%s\n",(const char*)varTree->toStringTree(varTree)->chars);
#endif
		GlocalVar *var = new GlocalVar(m_curfName, -1, getCurBlockFullName(), blockpath);

		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE)varTree->getChild(varTree, i);
			int type = child->getType(child);
			if(type==TYPE){
				//Primitive 타입인지 symbol 타입인지 파악
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
				unsigned int type2 = n->getType(n);
				if(type2==PTYP){
					pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE)n->getChild(n, 0);
					Gtoken *t = new Gtoken(m_curfName, getCurBlockFullName(), blockpath, (char*)n2->getText(n2)->chars, n2->getLine(n2), n2->getType(n2));
					var->setType(t);
					var->setAsPrim();
				}
				if(type2==STYP){
					pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE)n->getChild(n, 0);
					pANTLR3_BASE_TREE n3 = (pANTLR3_BASE_TREE)n2->getChild(n2, 0);
					Gtoken *t = new Gtoken(m_curfName, getCurBlockFullName(), blockpath, (char*)n3->getText(n3)->chars, n3->getLine(n3), n3->getType(n3));
					var->setType(t);
				}
			}
			if(type==NAME){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
				Gtoken *vname = new Gtoken(m_curfName, getCurBlockFullName(), blockpath, (char*)n->getText(n)->chars, n->getLine(n), n->getType(n));
				var->setName(vname);
			}
			if(type == RVAL){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);				
				GstmtExpr* rval = getExpr(blockpath, n);
				unsigned int line = n->getLine(n);
				rval->setEndLine(line);//값이 model함수 호출이면 위치를 알 필요가 있음
				//rval->print();
				var->setValueExpr(rval);
			}
			if(type == EOL){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
				unsigned int line = n->getLine(n);
				var->setEndLine(line);
				//block명 지정
				//std::string blockfullname = m_curWorkingBhv;
				//if(m_curWorkingState != "") blockfullname.append(".").append(m_curWorkingState); 
				//if(m_curWorkingBlock != "") blockfullname.append(".").append(m_curWorkingBlock); 
				//var->setBlockFullName(blockfullname);				
			}
		}

		return var;
	}
	return 0;
}

Gwaitfor* EParser::getWait(const std::string blockpath, pANTLR3_BASE_TREE varTree)
{
	unsigned int cNum = varTree->getChildCount(varTree);
	if(cNum < 1){
		return NULL;
	}
	else{

		Gwaitfor *w = new Gwaitfor(m_curfName, -1, getCurBlockFullName(), blockpath);

		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE)varTree->getChild(varTree, i);
			int type = child->getType(child);
			if(type==TIME){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
				GstmtExpr* time_expr = getExpr(blockpath, n);
				w->setTime(time_expr);
			}
			if(type==EOL){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
				unsigned int line = n->getLine(n);
				w->setEndLine(line);
				//block명 지정
				//std::string blockfullname = m_curWorkingBhv;
				//if(m_curWorkingState != "") blockfullname.append(".").append(m_curWorkingState); 
				//if(m_curWorkingBlock != "") blockfullname.append(".").append(m_curWorkingBlock); 
				//w->setBlockFullName(blockfullname);
			}
		}
		return w;
	}

	return 0;
}

Gsynch* EParser::getSynch(const std::string blockpath, pANTLR3_BASE_TREE varTree)
{
	unsigned int cNum = varTree->getChildCount(varTree);
	if(cNum < 1){
		return NULL;
	}
	else{

		Gsynch *w = new Gsynch(m_curfName, -1, getCurBlockFullName(), blockpath);

		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE)varTree->getChild(varTree, i);
			int type = child->getType(child);
			if(type==ID){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
				GstmtExpr* time_expr = getExpr(blockpath, n);
				w->setSynchID(time_expr);
			}
			if(type==EOL){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
				unsigned int line = n->getLine(n);
				w->setEndLine(line);
				//block명 지정
				//std::string blockfullname = m_curWorkingBhv;
				//if(m_curWorkingState != "") blockfullname.append(".").append(m_curWorkingState); 
				//if(m_curWorkingBlock != "") blockfullname.append(".").append(m_curWorkingBlock); 
				//w->setBlockFullName(blockfullname);
			}
		}
		return w;
	}

	return 0;
}

GstmtExpr* EParser::getExpr(const std::string blockpath, pANTLR3_BASE_TREE exprTree)
{
	unsigned int cNum = exprTree->getChildCount(exprTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		EXTree* eTree = getExprTree(blockpath, exprTree);

		#ifdef __DEBUG_ETree_
			printf("EParser::getExpr();\n");
			eTree->print();
		#endif

		GstmtExpr* expr = new GstmtExpr(m_curfName, -1, getCurBlockFullName(), blockpath);
		expr->setExprTree(eTree);
		return expr;
	}
}

EXTree* EParser::getExprTree(const std::string blockpath, pANTLR3_BASE_TREE exprTree)
{
	unsigned int cNum = exprTree->getChildCount(exprTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		//antlr에서 EXPR_ROOT 토큰이 선언되어 있다.(토큰만)
		Gtoken root = Gtoken(m_curfName, getCurBlockFullName(), blockpath, "EXPR_ROOT", 0, EXPR_ROOT);
		//EXTree* eTree = new ETree<Gtoken>(root);
		EXTree* eTree = new EXTree(root);
		//root에 새로운 ETr
		makeExprTree(blockpath, eTree, exprTree);
		return eTree;
	}
}

//void EParser::makeExprTree(const std::string blockpath, ETree<Gtoken>* eTree, pANTLR3_BASE_TREE tree)
void EParser::makeExprTree(const std::string blockpath, EXTree* eTree, pANTLR3_BASE_TREE tree)
{
	pANTLR3_BASE_TREE c = tree;

	while(isStepping(c)){ //중간에 의미없는 AND,OR, NOT, ADD, MUL등을 지나치게 한다.
		c = (pANTLR3_BASE_TREE) (c->getChild(c, 0));
	}

	if(c != NULL){

//이 부분은 아주 중요함 문법을 바꿀때.. 꼭 체크
//std::string temp = std::string((char*)c->toString(c)->chars);
		int cType = c->getType(c);
		if(cType==SYMB || cType==CNAME){//단말에 해당 a.b.c가 트리구조인데 이걸 하나의 string으로 생성한다.

			//SYMB 정보 제공 노드 생성
			Gtoken node1 = Gtoken(m_curfName, getCurBlockFullName(), blockpath, (char*) c->toString(c)->chars, c->getLine(c), c->getType(c));
			//ETree<Gtoken>* child1 = new ETree<Gtoken>(node1);
			EXTree* child1 = new EXTree(node1);

			//a, a.c, 100등에 정보를 담는 노드
			pANTLR3_BASE_TREE prim = (pANTLR3_BASE_TREE) c->getChild(c, 0);

			//트리 형태의 (ID (3(.(4)))->3.4로 변환
			std::string ts = symTree3str(prim);

			Gtoken node2 = Gtoken(m_curfName, getCurBlockFullName(), blockpath, ts.c_str(), prim->getLine(prim), prim->getType(prim));
			//ETree<Gtoken>* child2 = new ETree<Gtoken>(node2);
			EXTree* child2 = new EXTree(node2);

			//트리에 노드를 추가
			child1->addChild(child2);
			eTree->addChild(child1);
		}
		else if(cType == LITE){ //(LITE(LITE_BOO true))

			//LITE 정보 제공 노드 생성
			Gtoken node1 = Gtoken(m_curfName, getCurBlockFullName(), blockpath, (char*) c->toString(c)->chars, c->getLine(c), c->getType(c));
			//ETree<Gtoken>* child1 = new ETree<Gtoken>(node1);
			EXTree* child1 = new EXTree(node1);

			pANTLR3_BASE_TREE c2 = (pANTLR3_BASE_TREE) c->getChild(c, 0);
			pANTLR3_BASE_TREE c3 = (pANTLR3_BASE_TREE) c2->getChild(c2, 0);
			char *tStr1 = (char*)c2->toStringTree(c2)->chars;
			char *tStr2 = (char*)c3->toStringTree(c3)->chars;
			Gtoken node2;

			int cType2 = c2->getType(c2);
			if(cType2==LITE_DEC){
				node2 = Gtoken(m_curfName, getCurBlockFullName(), blockpath, tStr2, c->getLine(c), TYP_INT);
			}
			if(cType2==LITE_BOO){
				node2 = Gtoken(m_curfName, getCurBlockFullName(), blockpath, tStr2, c->getLine(c), TYP_BOOL);
			}
			if(cType2==LITE_FLO){
				node2 = Gtoken(m_curfName, getCurBlockFullName(), blockpath, tStr2, c->getLine(c), TYP_FLOAT);
			}
			if(cType2==LITE_STR){
				node2 = Gtoken(m_curfName, getCurBlockFullName(), blockpath, tStr2, c->getLine(c), TYP_STRING);
			}

			//ETree<Gtoken>* child2 = new ETree<Gtoken>(node2);
			EXTree* child2 = new EXTree(node2);

			child1->addChild(child2);
			eTree->addChild(child1);

		}

		else if(isNOTsym(c)){
			//'!'기호는 중복이기 때문에 건너뛴다.
		}
		else{ // 자식 노드가 존재하는 경우, AND, OR, NOT 등    ex) 1 + 3
			std::string data = std::string((char*)c->getText(c)->chars); //ex) ADD
			unsigned int dtype = getOpType(data); //ex) 15
			if(dtype == UNDEFINED_SYMBOL)
				dtype = c->getType(c);

			Gtoken node = Gtoken(m_curfName, getCurBlockFullName(), blockpath, (char*)c->getText(c)->chars, c->getLine(c), dtype);	//BOOL Gtoken
			//ETree<Gtoken>* child = new ETree<Gtoken>(node);
			EXTree* child = new EXTree(node);
			eTree->addChild(child);

			//AND 자식 노드에다수의 관계식 이 올 수 있다.
			for(unsigned int i=0; i<c->getChildCount(c) ;i++){  //ex) 1, +, 3 3개
				pANTLR3_BASE_TREE c2 = (pANTLR3_BASE_TREE) (c->getChild(c, i));
	std::string dbg = std::string((char*)c2->getText(c2)->chars); //ex) ADD
				makeExprTree(blockpath, child, c2);
			}
		}
	}
}

GstmtGotoState* EParser::getGotoState(const std::string blockpath, pANTLR3_BASE_TREE gotoTree)
{
	unsigned int cNum = gotoTree->getChildCount(gotoTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		GstmtGotoState *gotoStmt = new GstmtGotoState(m_curfName, -1, getCurBlockFullName(), blockpath);

		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE)gotoTree->getChild(gotoTree, i);
			//char *str = (char*)child->toString(child)->chars;
			//std::string sym(str);

			if(child->getType(child)==STATE){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE) n->getChild(n, 0);
char *tStr1 = (char*)n->toStringTree(n)->chars;
char *tStr2 = (char*)n2->toStringTree(n2)->chars;
				std::string ts = symTree3str(n);
				std::string gotoname = m_curWorkingNode;
				gotoname.append(".").append(ts);
				Gtoken *sName = new Gtoken(m_curfName, getCurBlockFullName(), blockpath, gotoname.c_str(), n2->getLine(n2), n2->getType(n2));
				gotoStmt->setStateName(sName);
			}
			if(child->getType(child)==STAY){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				Gtoken *sName = new Gtoken(m_curfName, getCurBlockFullName(), blockpath, (char*)n->getText(n)->chars, n->getLine(n), n->getType(n));
				gotoStmt->setStateName(sName);
			}
			if(child->getType(child)==EOL){				
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
				unsigned int line = n->getLine(n);
				gotoStmt->setEndLine(line);				
				//block명 지정
				//std::string blockfullname = m_curWorkingBhv;
				//if(m_curWorkingState != "") blockfullname.append(".").append(m_curWorkingState); 
				//if(m_curWorkingBlock != "") blockfullname.append(".").append(m_curWorkingBlock); 
				//gotoStmt->setBlockFullName(blockfullname);
			}
		}
		return gotoStmt;
	}
}

GstmtInvokeSub* EParser::getInvokeSub(const std::string blockpath, pANTLR3_BASE_TREE gotoTree)
{
	unsigned int cNum = gotoTree->getChildCount(gotoTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		GstmtInvokeSub *ivkcall = new GstmtInvokeSub(m_curfName, -1, getCurBlockFullName(), blockpath);		
		
		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE)gotoTree->getChild(gotoTree, i);

			if(child->getType(child)==BEHA){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				//pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE) n->getChild(n, 0);
				Gcall* bhv = getFuncall(blockpath, n);
				ivkcall->setInvokeCall(bhv);
			}
			if(child->getType(child)==EOE){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE) n->getChild(n, 0);
				std::string ts = symTree3str(n2);
				std::string gotoname = m_curWorkingNode;
				if(ts == "break")
					gotoname = ts;
				else
					gotoname.append(".").append(ts);
				Gtoken *ntName = new Gtoken(m_curfName, getCurBlockFullName(), blockpath, gotoname.c_str(), n2->getLine(n2), n2->getType(n2));
				ivkcall->setNextTarget(ntName);
			}
			if(child->getType(child)==EOB){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE) n->getChild(n, 0);
				std::string ts = symTree3str(n2);
				Gtoken *bvar = new Gtoken(m_curfName, getCurBlockFullName(), blockpath, ts.c_str(), n2->getLine(n2), n2->getType(n2));
				ivkcall->setBoolVar(bvar);
			}
			if(child->getType(child)==EOL){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
				unsigned int line = n->getLine(n);
				ivkcall->setEndLine(line);				
			}
		}
		return ivkcall;
	}
}

GstmtCall* EParser::getCallStmt(const std::string blockpath, pANTLR3_BASE_TREE callTree)
{
	unsigned int cNum = callTree->getChildCount(callTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		GstmtCall *callstmt = new GstmtCall(m_curfName, -1, getCurBlockFullName(), blockpath);
		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE)callTree->getChild(callTree, i);	

			int type = child->getType(child);
			if(type==CALL){
				Gcall* gcall = getFuncall(blockpath, child);
				callstmt->setCall(gcall);			
			}
			else if(type==EOL){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
				unsigned int line = n->getLine(n);
				callstmt->setEndLine(line);			
			}

		}
		return callstmt;
	}
}

Gcall* EParser::getFuncall(const std::string blockpath, pANTLR3_BASE_TREE callTree)
{
	unsigned int cNum = callTree->getChildCount(callTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		Gcall *call = new Gcall(m_curfName, -1, getCurBlockFullName(), blockpath);

		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE)callTree->getChild(callTree, i);
			//char *str = (char*)child->toString(child)->chars;
			//std::string sym(str);
			int type = child->getType(child);

			if(type==CNAME){
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE) n->getChild(n, 0);
				//트리구조의(ID a(.(b)))를 string형태 a.b로 변환
				std::string ts = symTree3str(n);
				Gtoken *cName = new Gtoken(m_curfName, getCurBlockFullName(), blockpath, ts.c_str(), n2->getLine(n2), n2->getType(n2));				
				call->setName(cName);
			}
			else if(type==CPARAMS){
				EList<GstmtExpr*>* exprs = getCallParams(blockpath, child);
				call->setParams(exprs);
			}
		}
		return call;
	}
}

EList<GstmtExpr*>* EParser::getCallParams(const std::string blockpath, pANTLR3_BASE_TREE cparamsTree)
{

	unsigned int cNum = cparamsTree->getChildCount(cparamsTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		EList<GstmtExpr*>* exprs = new EList<GstmtExpr*>();
		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE)cparamsTree->getChild(cparamsTree, i);
			GstmtExpr *expr = getExpr(blockpath, child);
			exprs->addTail(expr);
		}

		return exprs;
	}
}


GstmtIf* EParser::getIfstmt(const std::string blockpath, pANTLR3_BASE_TREE ifTree)
{
	unsigned int cNum = ifTree->getChildCount(ifTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		GstmtIf *ifstmt = new GstmtIf(m_curfName, -1, getCurBlockFullName(), blockpath);

		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE)ifTree->getChild(ifTree, i);
			int type = child->getType(child);

			//char *str = (char*)child->toString(child)->chars;
			//std::string sym(str);

			Gcondition* cond = NULL;

			if(type==COND){
				cond = new Gcondition(m_curfName, -1, getCurBlockFullName(), blockpath);
				GstmtExpr *expr = getExpr(blockpath, child);
				cond->setCondition(expr);

				//원래모양  
				#ifdef __DEBUG_EParser_
				printf("Expression tree of If statement condition...\n");
				expr->print();
				printf("Converting conditional statement to DNF ...\n");
				#endif

				//DNF로 변환
				ifstmt->setIfCondition(cond);
				cond->toDNF(blockpath);

				//변환 후 모양
				#ifdef __DEBUG_EParser_
				ifstmt->print(0);
				#endif
			}
			else if(type==CONDEOL){
				//line
				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
				unsigned int line = n->getLine(n);
				//cond->setEndLine(line);

				//cond를 if문의 eol로 사용 할 경우
				ifstmt->setEndLine(line);

			}
			else if(type==WHENT){
				//GstmtIfDo* ifdo = new GstmtIfDo(m_curfName, -1, BID++, getCurBlockFullName(), blockpath);
				//std::string ifid = ifdo->getBlockID();
				//std::string path = blockpath; //밖으로 빼지말것 (for문으로 아래에 잘못된 값 들어감)
				//path.append(".").append(ifid);
				
				//EList<Gstmt*>* ifBlock = getStmts(path, child);
				//ifdo->setStmts(ifBlock);
				//ifstmt->setIfAction(ifdo);
				
				//EOL
				/*
				if(ifBlock != NULL && ifBlock->getSize()>0){
					Gstmt* stmt = (Gstmt*)ifBlock->getHead();
					ifdo->setEndLine(stmt->getEndLine());
				}*/

				//EList<Gstmt*>* ifBlock = getStmts(blockpath, child);
				//ifBlock->getHead();
				//ifstmt->setIfAction((Gstmt*)ifBlock->getCurObject());
				//delete ifBlock;

				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				Gstmt* ifblock = getStmt(blockpath, n);
				ifstmt->setIfAction(ifblock);
			}
			else if(type==WHENF){
				/*
				GstmtIfElse* ifelse = new GstmtIfElse(m_curfName, -1, BID++, getCurBlockFullName(), blockpath);
				std::string elseid = ifelse->getBlockID();
				std::string path = blockpath;
				path.append(".").append(elseid);

				EList<Gstmt*>* elseBlock = getStmts(path, child);
				ifelse->setStmts(elseBlock);
				ifstmt->setElseAction(ifelse);
				//EOL
				EList<Gstmt*>* ifBlock = getStmts(path, child);
				if(ifBlock != NULL && ifBlock->getSize()>0){
					Gstmt* stmt = (Gstmt*)ifBlock->getHead();
					ifelse->setEndLine(stmt->getEndLine());
				}
				*/
				
				//EList<Gstmt*>* elseBlock = getStmts(blockpath, child);
				//elseBlock->getHead();
				//ifstmt->setElseAction((Gstmt*)elseBlock->getCurObject());
				//delete elseBlock;

				pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
				Gstmt* elseblock = getStmt(blockpath, n);
				ifstmt->setElseAction(elseblock);
			}
		}
		return ifstmt;
	}
}


/************
 Model 파일
 ***********/

Genum* EParser::getEnum(pANTLR3_BASE_TREE enumTree)
{
	unsigned int cNum = enumTree->getChildCount(enumTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		Genum *enum_ = new Genum();

		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) enumTree->getChild(enumTree, i);
			unsigned int type = child->getType(child);
			switch(type){
				case NAME:{
					//^(ID Identifier ('.' Identifier)*)
					pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0); //ID
					pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE)n->getChild(n, 0);
					//char *name = (char*)n2->toString(n2)->chars;
					//std::string eName(name);
					Gtoken *eName = new Gtoken(m_curfName, getCurBlockFullName(), "", (char*)n2->getText(n2)->chars, n2->getLine(n2), n2->getType(n2));					
					enum_->setName(eName);
					break;
				}
				case ELEM:{
					pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
					pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE)n->getChild(n, 0);
					//char *name = (char*)n2->toString(n2)->chars;
					//std::string elem(name);
					Gtoken *elem = new Gtoken(m_curfName, getCurBlockFullName(), "", (char*)n2->getText(n2)->chars, n2->getLine(n2), n2->getType(n2));					
					enum_->addElement(elem);
					break;
				}
				default:
					break;
			}
		}

		return enum_;
	}
}

Gtoken* EParser::getSymbol(pANTLR3_BASE_TREE tokenTree)
{
	std::string		symbol = "";
	unsigned int	symbolLine;
	unsigned int	symbolType;
	int s = tokenTree->getChildCount(tokenTree); // m1.test.comp 이면 child가 5개
	for(int k=0; k<s; k++){						
		pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE)tokenTree->getChild(tokenTree, k); 
		symbolLine = n2->getLine(n2);
		symbolType = n2->getType(n2);
		symbol = symbol.append((char*)n2->getText(n2)->chars);
	}	
	
	Gtoken* symbolToken = new Gtoken(m_curfName, getCurBlockFullName(), "", symbol, symbolLine, symbolType);	
	return symbolToken;
}

GmodelVar* EParser::getModelVar( pANTLR3_BASE_TREE elemTree)
{
	unsigned int cNum = elemTree->getChildCount(elemTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		GmodelVar* var = new GmodelVar(m_curfName, -1, "", "");

		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) elemTree->getChild(elemTree, i);
			unsigned int type = child->getType(child);
			switch(type){
			case VMOD:
				{
					pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
					char *str = (char*)n->toString(n)->chars;
					std::string mode(str);
					if(mode.compare("in")==0)
						var->setMode(G_VARIN);
					if(mode.compare("out")==0)
						var->setMode(G_VAROUT);
					if(mode.compare("gvar")==0)
						var->setMode(G_GVAR);
					if(mode.compare("wvar")==0)
						var->setMode(G_WVAR);
					break;
				}
			case TYPE:
				{
					//Primitive 타입인지 symbol 타입인지 파악
					pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
					unsigned int type2 = n->getType(n);
					switch(type2){
						case PTYP:{
							var->setAsPrim();
							pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE)n->getChild(n, 0);
							Gtoken *t = new Gtoken(m_curfName, getCurBlockFullName(), "",(char*)n2->getText(n2)->chars, n2->getLine(n2), n2->getType(n2));							
							//char *str = (char*)n2->toString(n2)->chars;
							//std::string t(str);
							var->setType(t);
							break;
						}
						case STYP:{
							pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE)n->getChild(n, 0);
							pANTLR3_BASE_TREE n3 = (pANTLR3_BASE_TREE)n2->getChild(n2, 0);
							//char *str = (char*)n3->toString(n3)->chars;
							//std::string t(str);
							Gtoken *t = new Gtoken(m_curfName, getCurBlockFullName(), "",(char*)n3->getText(n3)->chars, n3->getLine(n3), n3->getType(n3));							
							var->setType(t);
							break;
						}
						default:
							break;
					}
					break;
				}
			case NAME:
				{
					pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
					pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE)n->getChild(n, 0);
					Gtoken *t = new Gtoken(m_curfName, getCurBlockFullName(), "",(char*)n2->getText(n2)->chars, n2->getLine(n2), n2->getType(n2));					
					var->setName(t);
					break;
				}
			default:
				break;
			}
		}
		return var;
	}
}

Gparameter* EParser::getActionParam(pANTLR3_BASE_TREE paramTree)
{
	unsigned int cNum = paramTree->getChildCount(paramTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		Gparameter *param = new Gparameter();

		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) paramTree->getChild(paramTree, i);
			unsigned int type = child->getType(child);
			switch(type){
			case TYPE:
				{
					pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
					unsigned int type2 = n->getType(n);
					switch(type2){
						case PTYP:{
							param->setAsPrim();
							pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE)n->getChild(n, 0);
							Gtoken *typ = new Gtoken(m_curfName, getCurBlockFullName(), "", (char*)n2->getText(n2)->chars, n2->getLine(n2), n2->getType(n2));							
							param->setType(typ);
							break;
						}
						case STYP:{
							pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE)n->getChild(n, 0);
							pANTLR3_BASE_TREE n3 = (pANTLR3_BASE_TREE)n2->getChild(n2, 0);
							Gtoken *typ = new Gtoken(m_curfName, getCurBlockFullName(), "", (char*)n3->getText(n3)->chars, n3->getLine(n3), n3->getType(n3));						
							param->setType(typ);
							break;
						}
						default:
							break;
					}
					break;
				}
			case NAME:
				{
					pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
					pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE)n->getChild(n, 0);
					Gtoken *pName = new Gtoken(m_curfName, getCurBlockFullName(), "", (char*)n2->getText(n2)->chars, n2->getLine(n2), n2->getType(n2));					
					param->setName(pName);
					break;
				}
			default:
				break;
			}
		}

		return param;
	}
}

Gaction* EParser::getAction(pANTLR3_BASE_TREE actionTree)
{
	unsigned int cNum = actionTree->getChildCount(actionTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		Gaction *action = new Gaction(m_curfName, -1, "", "");

		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) actionTree->getChild(actionTree, i);
			unsigned int type = child->getType(child);
			switch(type){
			case NAME:
				{
					pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
					pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE)n->getChild(n, 0);
					//char *name = (char*)n2->toString(n2)->chars;
					//std::string aName(name);
					Gtoken *aName = new Gtoken(m_curfName, getCurBlockFullName(), "", (char*)n2->getText(n2)->chars, n2->getLine(n2), n2->getType(n2));				
					action->setName(aName);
					break;
				}
			case PARM:
				{
					Gparameter *param = getActionParam(child);
					action->addParameter(param);
					break;
				}
			default:
				break;
			}
		}

		return action;
	}
}

Gfunction* EParser::getModelFunc(pANTLR3_BASE_TREE funcTree)
{
	unsigned int cNum = funcTree->getChildCount(funcTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		Gfunction *function = new Gfunction(m_curfName, -1, "", "");

		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) funcTree->getChild(funcTree, i);
			unsigned int type = child->getType(child);
			switch(type){
			case RTN:
				{
					//Primitive 타입인지 symbol 타입인지 파악
					pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
					unsigned int type2 = n->getType(n);
					switch(type2){
						case PTYP:{
							//var->setAsPrim();
							pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE)n->getChild(n, 0);
							Gtoken *t = new Gtoken(m_curfName, getCurBlockFullName(), "", (char*)n2->getText(n2)->chars, n2->getLine(n2), n2->getType(n2));						
							function->setReturnType(t);
							break;
						}
						case STYP:{
							pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE)n->getChild(n, 0);
							pANTLR3_BASE_TREE n3 = (pANTLR3_BASE_TREE)n2->getChild(n2, 0);
							Gtoken *t = new Gtoken(m_curfName, getCurBlockFullName(), "", (char*)n3->getText(n3)->chars, n3->getLine(n3), n3->getType(n3));							
							function->setReturnType(t);
							break;
						}
					default:
						break;
					}

					break;
				}
			case NAME:
				{
					pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
					pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE)n->getChild(n, 0);
					//char *name = (char*)n2->toString(n2)->chars;
					//std::string aName(name);
					Gtoken *aName = new Gtoken(m_curfName, getCurBlockFullName(), "", (char*)n2->getText(n2)->chars, n2->getLine(n2), n2->getType(n2));					
					function->setName(aName);
					break;
				}
			case PARM:
				{
					Gparameter *param = getActionParam(child);
					function->addParameter(param);
					break;
				}
			case ACTION:
				{
					function->setAsAction();
					break;
				}
			default:
				break;
			}
		}
		return function;
	}
}

/*
Gworker* EParser::getWorker(pANTLR3_BASE_TREE workTree)
{
	unsigned int cNum = workTree->getChildCount(workTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		Gworker *worker = new Gworker(BID++, m_curfName);

		for(unsigned int i=0; i<cNum; i++){

			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) workTree->getChild(workTree, i);
			unsigned int type = child->getType(child);

			switch(type){
			case NAME:
				{
					pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
					pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE) n->getChild(n, 0);
					Gtoken *nToken = new Gtoken(m_curfName, "", (char*)n2->getText(n2)->chars, n2->getLine(n2), n2->getType(n2));						
					worker->setName(nToken);
					break;
				}
			case DESC:
				{
					pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
					Gtoken *dToken = new Gtoken(m_curfName, "", (char*)n->getText(n)->chars, n->getLine(n), n->getType(n));
					worker->setDesc(dToken);
					break;
				}
			case STARTBHV:
				{
					pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
					pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE) n->getChild(n, 0);
					Gtoken *token = new Gtoken(m_curfName, "", (char*)n2->getText(n2)->chars, n2->getLine(n2), n2->getType(n2));
					worker->setStartBhv(token);
					break;
				}		
			default:
				break;
			}
		}
		return worker;
	}
}
*/

Gtask* EParser::getTask(pANTLR3_BASE_TREE workTree)
{
	unsigned int cNum = workTree->getChildCount(workTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		int blockid = BID++;
		std::string blockpath = int2str(blockid);
		Gtask *task = new Gtask(m_curfName, -1, blockid, getCurBlockFullName(), blockpath);	

		for(unsigned int i=0; i<cNum; i++){

			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) workTree->getChild(workTree, i);
			unsigned int type = child->getType(child);

			switch(type){
			case NAME:
				{
					pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
					pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE) n->getChild(n, 0);
					m_curWorkingNode = std::string((char*)n2->getText(n2)->chars);
					Gtoken *nToken = new Gtoken(m_curfName, getCurBlockFullName(), "", getCurBlockFullName(), n2->getLine(n2), n2->getType(n2));						
					task->setName(nToken);
					task->setBlockName(m_curWorkingNode);
					break;
				}			
			case CONSTR:
				{
					GstmtBlock* iniBlock = getActionBlock(blockpath, child);
					task->setConstruct(iniBlock);	
					break;
				}
			case DEST:
				{
					GstmtBlock* finBlock = getActionBlock(blockpath, child);
					task->setDestruct(finBlock);
					break;
				}
			case RUN:
				{	
					//GstmtBlock* runBlock = getActionBlock(blockpath, child);	
					GstmtBlock* runBlock2 = getRunblock(blockpath, child);	
					task->setRunBlock(runBlock2);
					break;
				}
			case EOL:
				{
					pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
					unsigned int line = n->getLine(n);
					task->setEndLine(line);				
				}
			case VB:
				{
					//pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
					buildUpVarBlock(task->getBlockID(), child, task);				}
			default:
				break;
			}
		}
		m_curWorkingNode = "";
		return task;
	}
}

/*
Gtask* EParser::getTask(pANTLR3_BASE_TREE taskTree)
{
	unsigned int cNum = taskTree->getChildCount(taskTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		Gtask *task = new Gtask(BID++);
		std::string tid = task->getBlockID();
		//std::string path = blockpath;
		//path.append(".").append(sid);

		for(unsigned int i=0; i<cNum; i++){

			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) taskTree->getChild(taskTree, i);
			unsigned int type = child->getType(child);

			switch(type){
			case NAME:
				{			
					pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, 0);
					pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE) n->getChild(n, 0);
					Gtoken *nToken = new Gtoken(m_curfName, "", (char*)n2->getText(n2)->chars, n2->getLine(n2), n2->getType(n2));
					task->setName(nToken);
					break;
				}
			case STMTS:
				{	
					EList<Gstmt*>* stmts = getStmts(tid, child);
					if(stmts != NULL){
						task->setStmts(stmts);
					}									
					break;
				}
			
			default:
				break;
			}
		}
		return task;
	}
}
*/
/*
void EParser::updateInclude(pANTLR3_BASE_TREE inclTree, Ginclude* incl)
{
	unsigned int cNum = inclTree->getChildCount(inclTree);

	if(cNum < 1){
		return;
	}
	else{
		//WFNAME, FNAME,
		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) inclTree->getChild(inclTree, i);
			unsigned int cNum2 = child->getChildCount(child);

			if(cNum2 < 1){
				return;
			}
			else{
				for(unsigned int i=0; i<cNum2; i++){
					pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE) child->getChild(child, i);
					Gtoken *token = new Gtoken(m_curfName, getCurBlockFullName(), "", (char*)n->getText(n)->chars, n->getLine(n), n->getType(n));
					incl->addTail(token);
					//incl->addFilename(token);
				}
			}
		}
	}
}
*/

Gcontext* EParser::getContext(pANTLR3_BASE_TREE ctxTree)
{
	unsigned int cNum = ctxTree->getChildCount(ctxTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		Gcontext *ctx = new Gcontext();

		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) ctxTree->getChild(ctxTree, i);
			unsigned int type = child->getType(child);
			switch(type){
				case NAME:{
					pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
					pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE)n->getChild(n, 0);
					Gtoken *name = new Gtoken(m_curfName, getCurBlockFullName(), "",(char*)n2->getText(n2)->chars, n2->getLine(n2), n2->getType(n2));
					ctx->setName(name);
					break;
				}
//				case CTXRULE:{
//					Grule* rule = getRule(child);
//					ctx->addRule(rule);
//					break;
//				}
//				case CONTEXT:{
//					Gcontext *ctxNested = getContext(child);
//					ctx->addContext(ctxNested);
//					break;
//				}
				default:
					break;
			}
		}

		return ctx;
	}
}

/*
Grule* EParser::getRule(pANTLR3_BASE_TREE ruleTree)
{
	unsigned int cNum = ruleTree->getChildCount(ruleTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		Grule *rule = new Grule();

		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) ruleTree->getChild(ruleTree, i);
			unsigned int type = child->getType(child);
			switch(type){
				case NAME:{
					pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0);
					pANTLR3_BASE_TREE n2 = (pANTLR3_BASE_TREE)n->getChild(n, 0);
					Gtoken *t = new Gtoken(m_curfName, "",(char*)n2->getText(n2)->chars, n2->getLine(n2), n2->getType(n2));
					rule->setName(t);
					break;
				}
				case COND:{
					GstmtExpr *expr = getExpr("", child);
					rule->setCondition(expr);
					rule->toDNF();
					break;
				}
			}
		}

		return rule;
	}
}
*/
Gmodel* EParser::getModel(pANTLR3_BASE_TREE defTree)
{
	unsigned int cNum = defTree->getChildCount(defTree);

	if(cNum < 1){
		return NULL;
	}
	else{
		Gmodel *model = new Gmodel();

		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) defTree->getChild(defTree, i);
			unsigned int type = child->getType(child);
			switch(type){
				case NAME:{//모델의 이름
					pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)child->getChild(child, 0); //ID					
					Gtoken *name = getSymbol(n);			
					model->setName(name);
					break;
				}
				case VAR:{ //모델 내부의 변수 선언
					GmodelVar *var = getModelVar(child);
					model->addVariable(var);
					break;
				}
				case FUNC:{ //모델 내부에 함수 선언
					Gfunction *func = getModelFunc(child);
					model->addFunc(func);
					break;
				}
				case MODEL:{ //모델 내부에 모델이 선언
					Gmodel *model2 = getModel(child);
					model->addModel(model2);
					break;
				}
				default:
					break;
			}
		}
		return model;
	}
}


int EParser::parseScript(std::string fileName)
{
	pANTLR3_UINT8					fName;
    pANTLR3_INPUT_STREAM			input;
	pANTLR3_COMMON_TOKEN_STREAM     cts;

	fName = (pANTLR3_UINT8) fileName.c_str();
	input = antlr3AsciiFileStreamNew(fName);
	//input = antlr3FileStreamNew(fName, ANTLR3_ENC_8BIT); //3.4
	

    if ( input == NULL){
		fprintf(stderr, "Failed to open file %s\n", (char*)fName);
		return FILE_FAILED_TO_OPEN;
	}

	/**********
		LEXER
	***********/

    pRFSMLexer lxr = RFSMLexerNew(input);

	if ( lxr == NULL ){
		LOG_ERROR(log, "Unable to create the lexer due to malloc() failure");
		return LEXER_ERROR;
    }

	cts = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lxr));

	if ( cts == NULL ){
		LOG_ERROR(log, "Out of memory trying to allocate token stream\n");
		return LEXER_ERROR;
    }


	//cts = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, lxr->pLexer->tokSource); //1.1.7

	/*
	//이부분은 모든 토큰을 화면에 순차적으로 출력한다.
	pANTLR3_COMMON_TOKEN token;
	while ((token = cts->tstream->_LT(cts->tstream, 1))->getType(token) != ANTLR3_TOKEN_EOF)
    {
      printf("Token: %s\n", token->toString(token)->chars);
      cts->tstream->istream->consume(cts->tstream->istream);
    }



    if (cts == NULL){
		LOG_ERROR(log, "Out of memory trying to allocate token stream");
		return TOKEN_STREAM_ERROR;
    }
	*/

    /**********
	  PARSER
	***********/
	pRFSMParser psr	= RFSMParserNew(cts);

    if (psr == NULL){
		LOG_ERROR(log, "Out of memory trying to allocate parser");
		return PARSER_ERROR_OUT_OF_MEM;
    }

	/*********
	  Testing
	*********/
	/*
	RFSMParser_rfsmDef_return def = psr->rfsmDef(psr);

	unsigned int errorCount = psr->pParser->rec->getNumberOfSyntaxErrors(psr->pParser->rec);
	//unsigned int errorCount = psr->pParser->rec->errorCount; //1.1.7

	int errorCode = 0;
	if (errorCount > 0){
		LOG_ERROR(log, "The parser returned %d errors, tree walking aborted.", errorCount);
		errorCode =  PARSER_SYNTAX_ERROR;
    }
	else{
		errorCode = parseDef(def.tree);
	}
*/


	//RFSMParser_rfsmDef_return def = psr->rfsmDef(psr);
	RFSMParser_rfsm_return def = psr->rfsm(psr);
	int errorCount0 = lxr->pLexer->rec->getNumberOfSyntaxErrors(lxr->pLexer->rec);
	int errorCount = psr->pParser->rec->getNumberOfSyntaxErrors(psr->pParser->rec);
	//unsigned int errorCount = psr->pParser->rec->errorCount; //1.1.7

	int errorCode = 0;

	if (errorCount0 > 0){
		LOG_ERROR(log, "The lexer returned %d errors, loading work aborted.", errorCount0);
		errorCode = LEXER_ERROR;
	}
	if (errorCount > 0){
		std::string fname = "ParErr.txt";//RFSMException에서 만들어진 파일명이 ParErr.txt이다.
		FILE* err;
		err=fopen(fname.c_str(), "rb");
		//크기계산
		fseek(err, 0, SEEK_END ); 
		int size = ftell(err); 
		fseek(err, 0, SEEK_SET ); 
		
		//파일읽기
		char* pBuffer = new char[size+1]; 
		memset(pBuffer, 0, size+1);	
		fread(pBuffer, size, 1, err); 
		
		LOG_ERROR(log, "%s", pBuffer);

		//파일닫기
        fclose(err); 
        delete[] pBuffer; 
		remove(fname.c_str());

		LOG_ERROR(log, "The parser returned %d errors, loading work aborted.", errorCount);
		errorCode =  PARSER_SYNTAX_ERROR;
    }
	else{
		//printf((const char*)def.tree->toStringTree(def.tree)->chars);
		//unsigned int type = def.tree->getType(def.tree);
		//if(type == RTDL){
			//pANTLR3_BASE_TREE n = (pANTLR3_BASE_TREE)def.tree->getChild(def.tree,0);
errorCode = parseDef2(def.tree);
		//}
	}

	/*
	pANTLR3_COMMON_TOKEN theToken =(pANTLR3_COMMON_TOKEN)(psr->pParser->rec->state->exception->token);
	pANTLR3_STRING ttext  = theToken->toString(theToken);
	ANTLR3_FPRINTF(stderr, ", at offset %d", psr->pParser->rec->state->exception->charPositionInLine);
    if  (theToken != NULL){
        if (theToken->type == ANTLR3_TOKEN_EOF)
        {
            ANTLR3_FPRINTF(stderr, ", at <EOF>");
        }
        else
        {
			// Guard against null text in a token
			//
			ANTLR3_FPRINTF(stderr, "\n    near %s\n", ttext == NULL ? (pANTLR3_UINT8)"<no text for the token>" :ttext->chars);
        }
    }
*/

	psr	    ->free  (psr);			psr = NULL;
	cts		->free  (cts);			cts = NULL;
    lxr	    ->free  (lxr);			lxr = NULL;
    input   ->close (input);	    input = NULL;

	return errorCode;
}

int EParser::parseStrBlock(std::string exprString, int type, SObj* sobj, std::string name)
{
	if(exprString == "") return 0;
	Estring::replaceAll(exprString, "<", "");
	Estring::replaceAll(exprString, ">", "");
	Estring::trim(exprString);
    pANTLR3_INPUT_STREAM			input;
	pANTLR3_COMMON_TOKEN_STREAM     cts;

	input = antlr3NewAsciiStringInPlaceStream((pANTLR3_UINT8)exprString.c_str(), exprString.size(), (pANTLR3_UINT8)name.c_str());
	//input = antlr3StringStreamNew((pANTLR3_UINT8)exprString.c_str(), ANTLR3_ENC_8BIT, exprString.size(), (pANTLR3_UINT8)name.c_str()); //3.4

    if (input == NULL){
		fprintf(stderr, "Failed to make an input stream for %s\n", name.c_str());
	}

	/**********
		LEXER
	***********/
    pRFSMLexer lxr = RFSMLexerNew(input);

	if ( lxr == NULL ){
		LOG_ERROR(log, "Unable to create the lexer due to malloc() failure");
		return LEXER_ERROR;
    }

	cts = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lxr));

	if ( cts == NULL ){
		LOG_ERROR(log, "Out of memory trying to allocate token stream\n");
		return LEXER_ERROR;
    }

    /**********
	  PARSER
	***********/	
	pRFSMParser psr	= RFSMParserNew(cts);

    if (psr == NULL){
		LOG_ERROR(log, "Out of memory trying to allocate parser");
		return PARSER_ERROR_OUT_OF_MEM;
    }
	
	RFSMParser_expr_return def;
	RFSMParser_sce_act_stmts_return def2; 
	if(type == EVTEXPR)		def = psr->expr(psr);
	if(type == STRBLOCK)	def2= psr->sce_act_stmts(psr);

	int errorCount0 = lxr->pLexer->rec->getNumberOfSyntaxErrors(lxr->pLexer->rec);
	int errorCount	= psr->pParser->rec->getNumberOfSyntaxErrors(psr->pParser->rec);
	int errorCode	= 0;

	if (errorCount0 > 0){
		LOG_ERROR(log, "The lexer returned %d errors, loading work aborted.", errorCount0);
		errorCode = LEXER_ERROR;
	}
	if (errorCount > 0){
		std::string fname = "ParErr.txt";//RFSMException에서 만들어진 파일명이 ParErr.txt이다.
		FILE* err;
		err=fopen(fname.c_str(), "rb");
		//크기계산
		fseek(err, 0, SEEK_END ); 
		int size = ftell(err); 
		fseek(err, 0, SEEK_SET ); 
		
		//파일읽기
		char* pBuffer = new char[size+1]; 
		memset(pBuffer, 0, size+1);	
		fread(pBuffer, size, 1, err); 
		
		LOG_ERROR(log, "%s", pBuffer);

		//파일닫기
        fclose(err); 
        delete[] pBuffer; 
		remove(fname.c_str());

		LOG_ERROR(log, "The parser returned %d errors, Parsing a task scenario is aborted.", errorCount);
		errorCode =  PARSER_SYNTAX_ERROR;
    }
	//else{
		//printf((const char*)def.tree->toStringTree(def.tree)->chars);
		//unsigned int type = def.tree->getType(def.tree);
		

		if(type == EVTEXPR){
			Estring::replaceAll(exprString, "$", "");
			GstmtExpr*stmtExpr = getExpr("", def.tree);
			//stmtExpr->print();
			((SEvent*)sobj)->setCondition(stmtExpr);
			((SEvent*)sobj)->setCondStr(exprString);
		}
		if(type == STRBLOCK){
			EList<Gstmt*>*stmts = getStmts("", def2.tree);
			((SBehavior*)sobj)->setStmtList(stmts);
			((SBehavior*)sobj)->setStmtsStr(exprString);
		}
	//}

	psr	    ->free  (psr);			psr = NULL;
	cts		->free  (cts);			cts = NULL;
    lxr	    ->free  (lxr);			lxr = NULL;
    input   ->close (input);	    input = NULL;

	return errorCode;
}

/*
void EParser::setInclude(pANTLR3_BASE_TREE deftree, Ginclude* incl)
{
	unsigned int cNum = deftree->getChildCount(deftree);

	if(cNum < 1){
		return;
	}
	else{
		for(unsigned int i=0; i<cNum; i++){
			pANTLR3_BASE_TREE child = (pANTLR3_BASE_TREE) deftree->getChild(deftree, i);
			unsigned int type = child->getType(child);
			if(type==INCL){
				updateInclude(child, incl);
			}
		}
	}
}
*/
std::string ignoreComment(std::ifstream &ifs, char* curLine, int mode)
{
	std::string alstr = std::string(curLine);

	if(mode == 0){
		int s = alstr.find("/*");
		if(s == -1){
			int s2 = alstr.find("//");
			if(s2 == -1){//comment, line comment아무것도 없다.
				return std::string(curLine);
			}
			else{
				if(s2==0)//처음 부터 line comment
					return std::string("");
				else
					//뒷쪽에 line comment
					return alstr.substr(0, s2-1);
			}
		}
		else{
			int e = alstr.find("*/");
			if(e != -1){ //한줄에 /* */이 있는 경우
				std::string alstr2= alstr.substr(e+2, 225);
				return ignoreComment(ifs, (char*)alstr2.c_str(), 0);
			}
			else{//아래줄에 */가 있는 경우
				char line[225];
				if(ifs.getline(line, 225)){
					return ignoreComment(ifs, line, 1);
				}
			}
		}
	}
	else if(mode == 1){ // "*/"를 찾을 때 까지
		int e = alstr.find("*/");
		if(e != -1){//라인의 특정 위치에 있는 경우
			std::string alstr2= alstr.substr(e+2, 225);
			return ignoreComment(ifs, (char*)alstr2.c_str(), 0);
		}
		else{//라인에 없는 경우
			char line[225];
			if(ifs.getline(line, 225)){
				return ignoreComment(ifs, line, 1);
			}
		}
	}

	return alstr;
}

int EParser::extractInclude(std::string filename, std::string deployHome)
{
	if(filename.find_first_of("/")!=0){   // 현재는 '/'를 암묵적인 path로 이용하기 때문에(나중에 사용자 지정 path지원하면 이부분 수정)
		filename.insert(0, "/");
	}

	m_incl->checked(filename);			//검사 했다는 표시를 한다.
	m_incl->add(filename, filename);	//자신도 include 리스트에 넣는다.(file, incl)

	//std::string homeDir = TaskConfig::getHomeDir();

	std::string fullpathName;
	if(deployHome == "")
		fullpathName = getCurPath();	
	else
		fullpathName = deployHome;

	
	fullpathName.append(filename.c_str());

	// 입력파일
	std::ifstream ifs(fullpathName.c_str(), std::ifstream::in);

	if(!ifs){
		fprintf(stderr, "Parser can not open the file '%s'.", filename.c_str());
		LOG_ERROR(log, "Parser can not open the file '%s'.", filename.c_str());
		return FILE_FAILED_TO_OPEN;
	}

	char line[225];

	EList<std::string>* filelist = new EList<std::string>();

	// 라인단위로 파일을 읽어내기
	while( ifs.getline(line, 225)){
		std::string lstr = ignoreComment(ifs, line, 0);

		// 읽은 문자열이 "#include"라는 문자열을 포함하는가?
		//if(lstr.find("#")>=0 && lstr.find("include")>=0){
		if(strstr(lstr.c_str(), "#") && strstr(lstr.c_str(), "include")){
			Estring::replaceAll(lstr,"#"," ");
			Estring::replaceAll(lstr,"include"," ");
			Estring::replaceAll(lstr,"\""," ");
			lstr = Estring::trim(lstr);

			m_incl->add(m_curfName, lstr);
			//std::string ifname = homeDir + "/" + lstr;
			filelist->addTail(lstr);

			//std::vector<std::string> finfo;
			//finfo = Estring::split(lstr, ".");
		}
	}

	ifs.close();

	//include에 포함된 파일의 include도 조사
	int result = 0;
	if(filelist->moveHead()){
		do{
			std::string includedfile = filelist->getCurObject();
			if(!m_incl->isChecked(includedfile)){
				const char* ch =includedfile.c_str();
				
				//int mode = isAccessible(includedfile); //includedfile앞에 homeDir이 붙어야 한다.
				//if(mode == 0){
					//파일이 존재하면
					result = extractInclude(includedfile, deployHome);
					//include에 명기된 파일이 존재하지 않는 경우
					if(result != 0){
						//break;
						return result;
					}
				//}
				//else{					
				//	fprintf(stderr, "Parser can not open the included file %s.", ch);
				//	LOG_ERROR(log, "Parser can not open the included file %s.", ch);
				//	result =  FILE_FAILED_TO_OPEN;
				//}
			}
		}
		while(filelist->moveNext());
	}

	if(filelist != NULL){
		while(filelist->moveHead()){
			filelist->delHead();
		}
		delete filelist;		
	}

	return result;
}

void EParser::setFilename(std::string fName)
{
	m_curfName = fName;
}

Ginclude* EParser::getInclude()
{
	return m_incl;
}

GparsedResult* EParser::getParsingResult()
{
	return m_parsedResult;
}