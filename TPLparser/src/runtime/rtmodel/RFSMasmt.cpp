/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMasmt.h"

RFSMasmt::RFSMasmt(unsigned int op, RFSMvariable* lvar, RFSMvalue* rval, std::string file, int line): 
          RFSMvalue(lvar->getBlockID(), lvar->getBlockName(), file, line),
		  RFSMstmt(CLS_RFSMasmt, file, lvar->getBlockID(), lvar->getBlockName(), line)
{
	m_op = op;
	m_lValue = lvar;
	m_rValue = rval;
}

RFSMasmt:: ~RFSMasmt(void)
{
	if(m_lValue != 0) delete m_lValue;
	if(m_rValue != 0) delete m_rValue;
}

unsigned int RFSMasmt::getType()
{
	return m_lValue->getType();
}

RFSMvariable* RFSMasmt::getLvalue()
{
	return m_lValue;
}

RFSMvalue* RFSMasmt::getRvalue()
{
	return m_rValue;
}

unsigned int RFSMasmt::isAssignable()
{
	unsigned int lt = m_lValue->getType();
	unsigned int rt = m_rValue->getType();

	return assignable(lt,rt);
}

std::string RFSMasmt::toString()
{
	std::string str = "";
	str.append(m_lValue->toString()).append(" ").append(getOpStr(m_op)).append(" ").append(RFSMvalue::toString(m_rValue));
	return str;
}
