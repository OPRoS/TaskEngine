/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMvar.h"

RFSMvar::RFSMvar(unsigned int op, RFSMvariable* lvar, std::string file, int line): 
		  RFSMvalue(lvar->getBlockID(), lvar->getBlockName(), file, line),
		  RFSMstmt(CLS_RFSMvar, file, lvar->getBlockID(), lvar->getBlockName(), line)
{
	m_op = op;
	m_lValue = lvar;
}

RFSMvar:: ~RFSMvar(void)
{
	if(m_lValue != 0) delete m_lValue;
}

unsigned int RFSMvar::getType()
{
	return m_lValue->getType();
}

RFSMvariable* RFSMvar::getLvalue()
{
	return m_lValue;
}

std::string RFSMvar::toString()
{
	std::string str = "";
	str.append(m_lValue->toString()).append(" ").append(getOpStr(m_op));
	return str;
}
