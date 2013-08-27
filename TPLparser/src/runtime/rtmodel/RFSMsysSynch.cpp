/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMsysSynch.h"

RFSMsysSynch::RFSMsysSynch(std::string name, std::string file, std::string blockpath, std::string blockname, int line)
	:RFSMcall(name, CLS_RFSMsysSynch, file, blockpath, blockname, line)
{
}

RFSMsysSynch:: ~RFSMsysSynch(void)
{
	if(m_Value != 0) delete m_Value;	
}

void RFSMsysSynch::setValue(RFSMvalue* value)
{
	m_Value = value;
}

RFSMvalue* RFSMsysSynch::getValue()
{
	return m_Value;
}

std::string RFSMsysSynch::toString()
{
	return this->getName();
}

int RFSMsysSynch::getSynchID()
{
	RFSMliteral* rlite = (RFSMliteral*)m_Value;
	std::string data = rlite->getData();
	return str2int(data);
}