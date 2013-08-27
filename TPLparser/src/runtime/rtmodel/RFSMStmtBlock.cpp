/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMStmtBlock.h"

RFSMStmtBlock::RFSMStmtBlock(std::string cName, std::string file, std::string blockpath, std::string blockname, int period, int line)
	:RFSMstmt(cName, file, blockpath, blockname, line)
{
	m_stmts = NULL;	
}

RFSMStmtBlock:: ~RFSMStmtBlock(void)
{
	if(m_stmts != NULL){
		for(unsigned int i=0; i<m_stmts->size(); i++){
			delete (*m_stmts)[i];
		}
		delete m_stmts;
	}	
}

void RFSMStmtBlock::setStmtBlock(std::vector<RFSMstmt*>* stmts)
{
	m_stmts = stmts;
}

std::vector<RFSMstmt*>* RFSMStmtBlock::getStmtBlock()
{
	return m_stmts;
}

std::string RFSMStmtBlock::toString()
{
	std::string str;
	if(m_stmts != NULL){
		while(m_stmts->size()>0){
			str.append("{");
			for(unsigned int i=0 ; i<m_stmts->size(); i++){
				RFSMstmt* rstmt  = (*m_stmts)[i];
				str.append(rstmt->toString());
			}
			str.append("}");
		}
	}

	return str;
}