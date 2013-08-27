/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMifStmt.h"

RFSMifStmt::RFSMifStmt(std::string file, std::string blockpath, std::string blockname, int line)
	:RFSMstmt(CLS_RFSMifStmt, file, blockpath, blockname, line)
{
	m_cond = NULL;
	m_Tstmts = NULL;
	m_Fstmts = NULL;
}

RFSMifStmt:: ~RFSMifStmt(void)
{
	if(m_cond != NULL) delete m_cond;
	//if(m_Tstmts != NULL) delete m_Tstmts;
	//if(m_Fstmts != NULL) delete m_Fstmts;

	
	if(m_Tstmts != NULL){
		/*while(m_Tstmts->moveHead()){
			RFSMstmt* obj = m_Tstmts->getCurObject();
			delete obj;
			m_Tstmts->delHead();
		}*/
		for(unsigned int i=0 ; i<m_Tstmts->size(); i++){
			delete (*m_Tstmts)[i];
		}
		delete m_Tstmts;
	}
	if(m_Fstmts != NULL){
		/*while(m_Fstmts->moveHead()){
			RFSMstmt* obj = m_Fstmts->getCurObject();
			delete obj;
			m_Fstmts->delHead();
		}*/
		for(unsigned int i=0 ; i<m_Fstmts->size(); i++){
			delete (*m_Fstmts)[i];
		}
		delete m_Fstmts;
	}
	
}


void RFSMifStmt::setCondition(RFSMboolable* cond)
{
	m_cond = cond;
}

void RFSMifStmt::setTrueStmts(std::vector<RFSMstmt*>* tstmts)
{
	m_Tstmts = tstmts;
}

void RFSMifStmt::setFalseStmts(std::vector<RFSMstmt*>* fstmts)
{
	m_Fstmts = fstmts;
}

RFSMboolable* RFSMifStmt::getCondition()
{
	return m_cond;
}

std::vector<RFSMstmt*>* RFSMifStmt::getTrueStmts()
{
	return m_Tstmts;
}

std::vector<RFSMstmt*>* RFSMifStmt::getFalseStmts()
{
	return m_Fstmts;
}

std::string RFSMifStmt::toString()
{
	std::string str="";
	
	if(m_Tstmts != NULL){
		str.append("IF(");
		for(unsigned int i=0 ; i<m_Tstmts->size(); i++){
			RFSMstmt* rstmt  = (*m_Tstmts)[i];
			str.append(rstmt->toString());
		}
		str.append(")");
	}
	if(m_Fstmts != NULL){
		str.append("ELSE(");
		for(unsigned int i=0 ; i<m_Tstmts->size(); i++){
			RFSMstmt* rstmt  = (*m_Fstmts)[i];
			str.append(rstmt->toString());
		}
		str.append(")");
	}

	/*
	if(m_Tstmts != NULL){
		str.append("IF(");
		m_Tstmts->toString();
		str.append(")");
	}
	if(m_Fstmts != NULL){
		str.append("ELSE(");
		m_Fstmts->toString();
		str.append(")");
	}
	*/
	return str;
}
