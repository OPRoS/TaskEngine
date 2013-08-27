/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/Gaction.h"

Gaction::Gaction(std::string fname, int eol, std::string blockname, std::string blockpath):Ggram(fname, eol, blockname, blockpath)
{
	m_params = NULL;
	m_name	= NULL;
}

Gaction:: ~Gaction(void)
{	
	if(m_name != NULL) delete m_name;

	if(m_params != NULL){
		while(m_params->moveHead()){
			Gparameter* obj = m_params->getCurObject();
			delete obj;
			m_params->delHead();
		}
		delete m_params;
	}
}

void Gaction::setName(Gtoken* name)
{
	m_name = name;
}

void Gaction::setParameters(EList<Gparameter*>* params)
{
	delete m_params;
	m_params = params;
}

void Gaction::addParameter(Gparameter* param)
{
	if(m_params == NULL)
		m_params = new EList<Gparameter*>();
	m_params->addTail(param);
}

void Gaction::print(unsigned int indent)
{
}

EList<Gparameter*>* Gaction::getParameters()
{
	return m_params;
}

Gtoken* Gaction::getName()
{
	return m_name;
}
