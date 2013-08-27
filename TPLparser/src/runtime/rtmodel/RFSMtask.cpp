/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMtask.h"

RFSMtask::RFSMtask(std::string name, int nameLine, int endline, std::string blockID, std::string file): RFSMtreenode(name, nameLine, endline, blockID, file, false)
{
}

RFSMtask::~RFSMtask(void)
{
	if(m_runBlock != NULL){
		/*while(m_runBlock->moveHead()){
			RFSMstmt* obj = m_runBlock->getCurObject();
			delete obj;
			m_runBlock->delHead();
		}
		delete m_runBlock;*/

		for(unsigned int i=0 ; i<m_runBlock->size(); i++){
			delete (*m_runBlock)[i];
		}
		delete m_runBlock;
	}
}

void RFSMtask::setSynchidInTask(void)
{
	collectSynIDs(m_synchidInTask); 
}

void RFSMtask::setRunBlock(std::vector<RFSMstmt*>* runblock)
{
	m_runBlock = runblock;
}

std::vector<RFSMstmt*>* RFSMtask::getRunBlock()
{
	return m_runBlock;
}

std::set<int> RFSMtask::getTaskSynchIds(void)
{
	return m_synchidInTask;
}

int RFSMtask::getEOLrun()
{
	return m_EOLrun;
}

void RFSMtask::setEOLrun(int eol)
{
	m_EOLrun = eol;
}

void RFSMtask::extractStartNodes(std::vector<RFSMstmt*>* stmts, std::vector<std::string>& nodelist)
{
	if(stmts != NULL && stmts->size()>0){
		for(unsigned int i=0 ; i<stmts->size(); i++){
			RFSMstmt* stmt = (*stmts)[i];

			if(stmt->getClassName().compare(CLS_RFSMifStmt)==0){
				RFSMifStmt* ifstmt = (RFSMifStmt*)stmt;
				
				//평가에 해당되는 statements
				std::vector<RFSMstmt*>* bStmts = NULL;
				bStmts = ifstmt->getTrueStmts();
				extractStartNodes(bStmts, nodelist);
				bStmts = ifstmt->getFalseStmts();
				extractStartNodes(bStmts, nodelist);

			}		
			else if(stmt->getClassName().compare(CLS_RFSMasmt)==0){
				//간과
			}			
			else if(stmt->getClassName().compare(CLS_RFSMcallFunc)==0){
				//간과
			}
			//subBehavior를 호출하는 경우
			else if(stmt->getClassName().compare(CLS_RFSMcallBehavior)==0){				
				RFSMcallBhv* bhv = (RFSMcallBhv*)stmt;
				std::string bhvname = bhv->getName();
				//bhv->getNextTarget();
				nodelist.push_back(bhvname);			
			}
			else if(stmt->getClassName().compare(CLS_RFSMcallConnector)==0){
								
				RFSMcallConnector* con = (RFSMcallConnector*)stmt;
				std::string conname = con->getName();
				//con->getNextTarget();
				nodelist.push_back(conname);

			}
			//다른 state로 전이되는 경우
			else if(stmt->getClassName().compare(CLS_RFSMgotoStmt)==0){
				
			}
			else if(stmt->getClassName().compare(CLS_RFSMsymbStmt)==0){
				//그냥 변수명만 a;
			}

			else if(stmt->getClassName()==CLS_RFSMblockStmt){
				RFSMStmtBlock* bstmt = (RFSMStmtBlock*)stmt;
				extractStartNodes(bstmt->getStmtBlock(), nodelist);
			}

			else if(stmt->getClassName()==CLS_RFSMparStmt){
				//error
			}
			else if(stmt->getClassName()==CLS_RFSMwithStmt){
			
			}
			else{
				//나머지는 error
			}
			//stay이 문에 대해서 .....

		}
	}	
}