/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMsign.h"

RFSMsign::RFSMsign(bool hasMinus, RFSMvalue* value, std::string file, int line)
	:RFSMvalue(value->getBlockID(), value->getBlockName(), file, line)
{
	m_hasMinus = hasMinus;
	m_value = value;
}

RFSMsign:: ~RFSMsign(void)
{
	if(m_value != NULL){
		delete m_value;
		m_value = NULL;
	}
}

void RFSMsign::setMinus()
{
	m_hasMinus = true;
}

bool RFSMsign::hasMinus()
{
	return m_hasMinus;
}


RFSMvalue* RFSMsign::getValue()
{
	return m_value;
}

unsigned int RFSMsign::getType()
{
	return m_value->getType();
}
