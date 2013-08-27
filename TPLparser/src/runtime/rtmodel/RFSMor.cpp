/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMor.h"

RFSMor::RFSMor(void): RFSMboolable(CLS_RFSMor)
{
}

RFSMor:: ~RFSMor(void)
{
}

std::string RFSMor::toString()
{
	std::string str = "OR ";
	return str.append(RFSMlogicOp::toString(this));
}
