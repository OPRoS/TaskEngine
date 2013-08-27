/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMsystem.h"

RFSMsystem::RFSMsystem(std::string blockpath, std::string blockname, unsigned int type, std::string name, std::string file, int line)
	:RFSMsym(blockpath, blockname, name, file, line)
{
	m_type = type;
}

RFSMsystem:: ~RFSMsystem(void)
{
}


unsigned int RFSMsystem::getType()
{
	return m_type;
}

std::string RFSMsystem::toString()
{
	return this->getName();
}
