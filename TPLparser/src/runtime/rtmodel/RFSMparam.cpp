/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMparam.h"

RFSMparam::RFSMparam(std::string name, RFSMvalue* value)
{
	m_name = name;
	m_value = value;
}

RFSMparam:: ~RFSMparam(void)
{
	if(m_value != NULL) delete m_value;
}

std::string RFSMparam::getName()
{
	return m_name;
}

RFSMvalue* RFSMparam::getValue()
{
	return m_value;
}
