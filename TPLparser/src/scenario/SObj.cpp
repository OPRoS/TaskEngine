/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2011.08.16
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "scenario/SObj.h"

SObj::SObj()
{
	m_eol = -1;
	m_name = NULL;
	m_desc = NULL;
}

SObj:: ~SObj(void)
{
	if(m_name != NULL) delete m_name;
	if(m_desc != NULL) delete m_desc;
}

Gtoken*	SObj::getName()
{
	return m_name;
}

Gtoken* SObj::getExpr()
{
	return m_desc;
}

int	SObj::getEOL()
{
	return m_eol;
}

void SObj::setName(Gtoken* name)
{
	m_name = name;
}

void SObj::setDesc(Gtoken* desc)
{
	m_desc = desc;
}

void SObj::setEOL(int eol)
{
	m_eol = eol;
}