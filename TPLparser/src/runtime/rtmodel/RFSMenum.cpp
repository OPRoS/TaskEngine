/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMenum.h"

RFSMenum::RFSMenum(int value, unsigned int type, std::string typestr, std::string name, std::string file, int line)
	:RFSMsym("", "", name, file, line)
{
	m_value = value;
	m_type = type;
	m_typeStr = typestr;
}

RFSMenum:: ~RFSMenum(void)
{
}

int RFSMenum::getValue()
{
	return m_value;
}

unsigned int RFSMenum::getType()
{
	return m_type;
}

std::string RFSMenum::getTypeStr()
{
	return m_typeStr;
}

std::string RFSMenum::toString()
{
	return this->getName();
}
