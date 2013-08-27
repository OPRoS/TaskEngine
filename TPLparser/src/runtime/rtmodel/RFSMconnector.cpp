/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMconnector.h"


RFSMconnector::RFSMconnector(int conType, std::string name, int nameLine, int endline, std::string blockID, std::string file, bool isMission)
	: RFSMtreenode(name, nameLine, endline, blockID, file, isMission)
{
	m_conType		= conType;
	m_withsBlock	= NULL;	
	//m_withIDs	= NULL;
}

RFSMconnector:: ~RFSMconnector(void)
{
	/*if(m_withStmts != NULL){
		while(m_withStmts->getSize()>0 && m_withStmts->moveHead()){
			RFSMstmt* obj = m_withStmts->getCurObject();
			delete obj;
			m_withStmts->delHead();
		}
		delete m_withStmts;
	}*/
	
	/*
	if(m_withStmts != NULL){
		for(unsigned int i=0 ; i<m_withStmts->size(); i++){
				delete (*m_withStmts)[i];
		}
		delete m_withStmts;
	}
	*/
	delete m_withsBlock;

	
	//if(m_withIDs != NULL){
		/*while(m_withIDs->getSize()>0 && m_withIDs->moveHead()){		
			m_withIDs->delHead();
		}*/
	//	delete m_withIDs;
	//}
}

void RFSMconnector::setJoinType(int type)
{
	m_joinType = type;
}

void RFSMconnector::setRunType(int type)
{
	m_runType = type;
}

void RFSMconnector::setSynchInfo(std::map<int, int> synchInfo)
{
	m_SynchIdMap = synchInfo;
}

void RFSMconnector::setRunBlocks(RFSMStmtBlock* withStmts)
{
	m_withsBlock = withStmts;
}

RFSMStmtBlock* RFSMconnector::getRunBlocks()
{
	return m_withsBlock;
}

int RFSMconnector::getJoinType()
{
	return m_joinType;
}

int RFSMconnector::getRunType()
{
	return m_runType;
}

int RFSMconnector::getConType()
{
	return m_conType;
}

std::map<int, int>*	RFSMconnector::getSynchIdMap()
{
	return &m_SynchIdMap;
}