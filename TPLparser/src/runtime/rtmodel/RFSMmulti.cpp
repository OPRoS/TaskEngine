/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMmulti.h"

RFSMmulti::RFSMmulti(unsigned int op, RFSMvalue* left, RFSMvalue* right, std::string file, int line)
	:RFSMvalue(left->getBlockID(), left->getBlockName(), file, line), 
	 RFSMbinaryOp(op, left, right)
{
}

RFSMmulti:: ~RFSMmulti(void)
{
}

unsigned int RFSMmulti::getType()
{
	return computationType(m_lval->getType(), m_rval->getType());
}

std::string RFSMmulti::toString()
{
	std::string rtn ="MULTI ";
	rtn.append(((RFSMbinaryOp*)this)->toString());
	return rtn;

}
