/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMcallAction.h"

RFSMcallAction::RFSMcallAction(std::string name, std::string file, std::string blockpath, std::string blockname, int line)
	:RFSMcall(name, CLS_RFSMcallAction, file, blockpath, blockname, line)
{

}

RFSMcallAction:: ~RFSMcallAction(void)
{
}

std::string RFSMcallAction::toString()
{
	return this->getName();
}
