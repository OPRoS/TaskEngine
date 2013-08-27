/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMcallConnector.h"

RFSMcallConnector::RFSMcallConnector(std::string name, std::string nextTarget, std::string file, std::string blockpath, std::string blockname, int period, int line)
	:RFSMcall(name, CLS_RFSMcallConnector, file, blockpath, blockname, line)
{
	m_nextTarget = nextTarget;
	m_period = period;
}

RFSMcallConnector:: ~RFSMcallConnector(void)
{
}

std::string RFSMcallConnector::getNextTargetState()
{
	return m_nextTarget;
}

std::string RFSMcallConnector::toString()
{
	return this->getName();
}

int RFSMcallConnector::getPeriod()
{
	return m_period;
}
