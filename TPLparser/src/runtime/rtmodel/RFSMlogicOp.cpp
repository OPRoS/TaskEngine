/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMlogicOp.h"

RFSMlogicOp::RFSMlogicOp()
{
	m_List = NULL;
}

RFSMlogicOp:: ~RFSMlogicOp(void)
{
	if(m_List != NULL){
		/*while(m_List->moveHead()){
			RFSMboolable* obj = m_List->getCurObject();
			delete obj;
			m_List->delHead();
		}*/
		for(unsigned int i=0 ; i<m_List->size(); i++){
			delete (*m_List)[i];
		}
		delete m_List;
	}
}

void RFSMlogicOp::addBoolExpr(RFSMboolable* elem)
{
	if(m_List == 0)
		m_List = new std::vector<RFSMboolable*>();
	m_List->push_back(elem);
}

std::vector<RFSMboolable*>* RFSMlogicOp::getBoolExprs()
{
	return m_List;
}

std::string RFSMlogicOp::toString(RFSMlogicOp* lop)
{
	std::string str = "";
	std::vector<RFSMboolable*>* list = lop->getBoolExprs();
	if(list != NULL){
		for(unsigned int i=0 ; i<list->size(); i++){
			RFSMboolable* b = (*list)[i];
			str.append(" ").append(RFSMboolable::toString(b));
		}
	}
	return str;
}
