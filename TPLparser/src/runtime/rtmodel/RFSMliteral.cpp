/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMliteral.h"

RFSMliteral::RFSMliteral(std::string blockPath, std::string blockname, std::string data, unsigned int type, std::string file, int line)
	:RFSMvalue(blockPath, blockname, file, line), 
	 RFSMboolable(CLS_RFSMliteral)
{
	m_data = data;
	m_type = type;
}

RFSMliteral:: ~RFSMliteral(void)
{
}

unsigned int RFSMliteral::getType()
{
	return m_type;
}

std::string RFSMliteral::getData()
{
	return m_data;
}

std::string RFSMliteral::toString()
{
	return m_data;
}
