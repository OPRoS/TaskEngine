/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMcallFunc.h"

RFSMcallFunc::RFSMcallFunc(std::string blockpath, std::string blockname, std::string name, bool isAction, std::string file, int line)
	:RFSMcall(name, CLS_RFSMcallFunc, file, blockpath, blockname, line), 
	 RFSMvalue(blockpath, blockname, file, line), 
	 RFSMboolable(CLS_RFSMcallFunc)
{
	m_isAction = isAction;
	//this->setEndLine(line);
}

RFSMcallFunc:: ~RFSMcallFunc(void)
{
}

unsigned int RFSMcallFunc::getType()
{
	return m_rtnType;
}

void RFSMcallFunc::setRtnType(unsigned int rtnType)
{
	m_rtnType = rtnType;
}

std::string RFSMcallFunc::toString()
{
	return this->getName();
}

bool RFSMcallFunc::isAction()
{
	return m_isAction;
}
