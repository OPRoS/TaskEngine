/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/Gfunction.h"

Gfunction::Gfunction(std::string file, int eol, std::string blockname, std::string blockpath): Gaction(file, eol, blockname, blockpath)
{
	m_isAction = false;
}

Gfunction:: ~Gfunction(void)
{
	if(m_rtnType != NULL)
		delete m_rtnType;
}

void Gfunction::setReturnType(Gtoken* rtnType)
{
	m_rtnType = rtnType;
}

Gtoken* Gfunction::getReturnType()
{
	return m_rtnType;
}

void Gfunction::setAsAction()
{
	m_isAction = true;
}

bool Gfunction::isAction()
{
	return m_isAction;
}