/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMcallBhv.h"

RFSMcallBhv::RFSMcallBhv(std::string name, std::string nextTarget, std::string file, std::string blockpath, std::string blockname, int period, int line)
	:RFSMcall(name, CLS_RFSMcallBehavior, file, blockpath, blockname, line)
{
	m_nextTarget = nextTarget;
	m_period = period;
}

RFSMcallBhv:: ~RFSMcallBhv(void)
{
}

std::string RFSMcallBhv::getNextTarget()
{
	return m_nextTarget;
}

std::string RFSMcallBhv::toString()
{
	return this->getName();
}

int RFSMcallBhv::getPeriod()
{
	return m_period;
}
