/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMcall.h"

RFSMcall::RFSMcall(std::string name, std::string className, std::string file, std::string blockpath, std::string blockname, int line)
	:RFSMstmt(className, file, blockpath, blockname, line), 
	 RFSMnamedItem(name)
{
	m_params= NULL;
}

RFSMcall:: ~RFSMcall(void)
{
	if(m_params != NULL){
		std::vector<RFSMparam*>::iterator it;
		for(it=m_params->begin() ; it!=m_params->end(); ++it){
			delete (*it);
		}
		delete m_params;
	}
}

void RFSMcall::addParameter(RFSMparam* param)
{
	if(m_params == NULL)
		m_params = new std::vector<RFSMparam*>();
	m_params->push_back(param);
}

std::vector<RFSMparam*>* RFSMcall::getParameters()
{
	return m_params;
}

std::string RFSMcall::toString()
{
	return this->getName();
}
