/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/symtab/TABcall.h"

TABcall::TABcall(std::string name, EList<TABparameter*>* params):RFSMnamedItem(name)
{
	m_params = params;	
}

TABcall:: ~TABcall(void)
{
	if(m_params != NULL){
		while(m_params->moveHead()){
			TABparameter* obj = m_params->getCurObject();
			delete obj;
			m_params->delHead();
		}
		delete m_params;
	}
}

EList<TABparameter*>* TABcall::getParameters()
{
	return m_params;
}

void TABcall::addParameter(TABparameter* param)
{
	if(m_params == 0)
		m_params = new EList<TABparameter*>();
	m_params->addTail(param);
}

TABparameter* TABcall::getParameter(std::string paramName)
{
	if(m_params != NULL){
		if(m_params->moveHead()){
			do{
				TABparameter* tp = m_params->getCurObject();
				std::string pName = tp->getName();
				if(pName.compare(paramName)==0)return tp;				
			}
			while(m_params->moveNext());
		}
	}
	return NULL;
}

void TABcall::setParameters(EList<TABparameter*>* params)
{
	if(m_params != NULL){
		while(m_params->moveHead()){
			TABparameter* obj = m_params->getCurObject();
			delete obj;
			m_params->delHead();
		}
		delete m_params;
	}

	m_params = params;	
}