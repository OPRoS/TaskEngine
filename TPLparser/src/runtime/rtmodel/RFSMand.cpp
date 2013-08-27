/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMand.h"

RFSMand::RFSMand(void): RFSMboolable(CLS_RFSMand)
{
}

RFSMand:: ~RFSMand(void)
{
}

std::string RFSMand::toString()
{
	std::string str = "AND ";
	return str.append(RFSMlogicOp::toString(this));
}
