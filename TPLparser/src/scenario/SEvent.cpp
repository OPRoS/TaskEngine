/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2011.08.16
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "scenario/SEvent.h"

SEvent::SEvent(void)
{
	m_cond = NULL;
}

SEvent::~SEvent(void)
{
	if(m_cond != NULL) delete m_cond;
}

void SEvent::setCondition(GstmtExpr* cond)
{
	m_cond = cond;
}

void SEvent::setCondStr(std::string condstr)
{
	m_condStr = condstr;
}

GstmtExpr* SEvent::getCondition()
{
	return m_cond;
}

std::string SEvent::getCondStr()
{
	return m_condStr;
}