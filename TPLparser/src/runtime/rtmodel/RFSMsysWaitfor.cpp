/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMsysWaitfor.h"

RFSMsysWaitfor::RFSMsysWaitfor(std::string name, std::string file, std::string blockpath, std::string blockname, int line)
	:RFSMcall(name, CLS_RFSMsysWaitfor, file, blockpath, blockname, line)
{
//	m_Value = val;
	m_timeout = 0;
}

RFSMsysWaitfor:: ~RFSMsysWaitfor(void)
{
	if(m_Value != 0) delete m_Value;	
}

void RFSMsysWaitfor::setValue(RFSMvalue* value)
{
	m_Value = value;
}

RFSMvalue* RFSMsysWaitfor::getValue()
{
	return m_Value;
}

std::string RFSMsysWaitfor::toString()
{
	return this->getName();
}