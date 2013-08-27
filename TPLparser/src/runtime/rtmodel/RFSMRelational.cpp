/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMRelational.h"

RFSMRelational::RFSMRelational(unsigned int op, RFSMvalue* lval, RFSMvalue* rval): RFSMbinaryOp(op, lval, rval), RFSMboolable(CLS_RFSMRelational)
{
}

RFSMRelational:: ~RFSMRelational(void)
{
}



std::string RFSMRelational::toString()
{
	 RFSMbinaryOp* bop =  (RFSMbinaryOp*)this;
	 return bop->toString();
}
