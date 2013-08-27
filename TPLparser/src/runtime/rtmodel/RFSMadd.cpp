/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMadd.h"

RFSMadd::RFSMadd(unsigned int op, RFSMvalue* left, RFSMvalue* right, std::string file, int line)
	:RFSMvalue(left->getBlockID(), left->getBlockName(), file, line), 
	 RFSMbinaryOp(op, left, right)
{
}

RFSMadd::~RFSMadd(void)
{
}

/*
bool RFSMadd::isTypeComputable()
{
	unsigned int lt = m_lOprend->getType();
	unsigned int rt = m_rOprend->getType();

	return isComputable(lt, rt);
}
*/
unsigned int RFSMadd::getType()
{
	return computationType(m_lval->getType(), m_rval->getType());
}

std::string RFSMadd::toString()
{
	std::string rtn ="ADD ";
	rtn.append(((RFSMbinaryOp*)this)->toString());
	return rtn;

}
