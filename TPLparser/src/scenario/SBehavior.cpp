/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2011.08.16
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "scenario/SBehavior.h"

SBehavior::SBehavior(void)
{
	m_stmtList = NULL;
}

SBehavior::~SBehavior(void)
{
	if(m_stmtList != NULL){
		while(m_stmtList->getHead() !=NULL && m_stmtList->moveHead()){
			Gstmt* obj = m_stmtList->getCurObject();
			delete obj;
			m_stmtList->delHead();
		}
		delete m_stmtList;
	}

	m_stmtList = NULL;
}

void SBehavior::setStmtList(EList<Gstmt*>* stmtlist)
{
	m_stmtList = stmtlist;
}

void SBehavior::setStmtsStr(std::string stmtsStr)
{
	m_stmtsStr = stmtsStr;
}

EList<Gstmt*>* SBehavior::getStmtList()
{
	return m_stmtList;
}

std::string SBehavior::getStmtStr()
{
	return m_stmtsStr;
}