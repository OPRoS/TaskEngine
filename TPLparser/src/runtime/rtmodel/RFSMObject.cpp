/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMObject.h"

RFSMObject::RFSMObject(std::string className)
{
	m_className = className;
}

RFSMObject:: ~RFSMObject(void)
{
}

std::string RFSMObject::getClassName()
{
	return m_className;
}
