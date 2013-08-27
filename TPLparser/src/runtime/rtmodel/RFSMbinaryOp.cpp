/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMbinaryOp.h"

RFSMbinaryOp::RFSMbinaryOp(unsigned int op, RFSMvalue* left, RFSMvalue* right)
{
	m_op = op;
	m_lval = left;
	m_rval = right;
}

RFSMbinaryOp:: ~RFSMbinaryOp(void)
{
	if(m_lval != 0) delete m_lval;
	if(m_rval != 0) delete m_rval;
}

unsigned int RFSMbinaryOp::getOperator()
{
	return m_op;
}

RFSMvalue* RFSMbinaryOp::getLeftOperand()
{
	return m_lval;
}

RFSMvalue* RFSMbinaryOp::getRightOperand()
{
	return m_rval;
}

std::string RFSMbinaryOp::toString()
{
	std::string rtn ="";

	rtn.append(RFSMvalue::toString(m_lval));
	rtn.append(getOpStr(m_op));
	rtn.append(RFSMvalue::toString(m_rval));

	return rtn;

}
