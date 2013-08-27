/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/symtab/TABfunction.h"

TABfunction::TABfunction(unsigned int rType, std::string name, EList<TABparameter*>* params):TABcall(name, params)
{
	m_rtnType = rType;
	m_isAction = false;
}

TABfunction:: ~TABfunction(void)
{
}

unsigned int TABfunction::getRtnType()
{
	return m_rtnType;
}

void TABfunction::setRtnType(unsigned int rtnType)
{
	m_rtnType = rtnType;
}


void TABfunction::setAsAction()
{
	m_isAction = true;
}

bool TABfunction::isAction()
{
	return m_isAction;
}