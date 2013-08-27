/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/Gcall.h"

Gcall::Gcall(std::string file, int eol, std::string blockname, std::string blockpath):Ggram(file, eol, blockname, blockpath)
{
	m_params = NULL;
	m_name = NULL;
}

Gcall:: ~Gcall()
{
	if(m_name != NULL)
		delete m_name;

	if(m_params != NULL){
		while(m_params->moveHead()){
			GstmtExpr* obj = m_params->getCurObject();
			delete obj;
			m_params->delHead();
		}
		delete m_params;
	}
}

void Gcall::setName(Gtoken* name)
{
	m_name = name;
}

void Gcall::setParams(EList<GstmtExpr*>* params)
{
	if(m_params != NULL){
		while(m_params->moveHead()){
			GstmtExpr* obj = m_params->getCurObject();
			delete obj;
			m_params->delHead();
		}
		delete m_params;
	}

	m_params = params;
}

Gtoken* Gcall::getName()
{
	return m_name;
}


EList<GstmtExpr*>*  Gcall::getParameters()
{
	return m_params;
}

void Gcall::print(unsigned int indent)
{
	titlePrint(indent, std::string("CALL"));

	if(m_name != NULL) m_name->print(indent+1);

	if(m_params !=NULL && m_params->moveHead()){
		do{
			GstmtExpr *expr = m_params->getCurObject();
			expr->print(indent+1);
		}
		while(m_params->moveNext());
	}
}