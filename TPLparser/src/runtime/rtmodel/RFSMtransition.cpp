/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMtransition.h"

RFSMtransition::RFSMtransition(void)
{
	m_ifstmts = NULL;
}

RFSMtransition:: ~RFSMtransition(void)
{
	if(m_ifstmts != NULL){
		/*while(m_ifstmts->moveHead()){
			RFSMifStmt* obj = m_ifstmts->getCurObject();
			delete obj;
			m_ifstmts->delHead();
		}*/
		for(unsigned int i=0; i<m_ifstmts->size();i++){
			delete (*m_ifstmts)[i];
		}
		delete m_ifstmts;
	}
}

void RFSMtransition::setIfstmts(std::vector<RFSMifStmt*>* ifstmts)
{
	if(m_ifstmts != NULL){
		for(unsigned int i=0; i<m_ifstmts->size();i++){
			delete (*m_ifstmts)[i];
		}
		delete m_ifstmts;
	}

	m_ifstmts = ifstmts;
}

std::vector<RFSMifStmt*>* RFSMtransition:: getIfstmts()
{
	return m_ifstmts;
}
