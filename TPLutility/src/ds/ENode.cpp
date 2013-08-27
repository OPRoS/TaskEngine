/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "ds/ENode.h"

ENode::ENode(void)
{
}

ENode:: ~ENode(void)
{
}

void ENode::setValue(char *value)
{
	m_value = value;
}

char* ENode::getValue()
{
	return m_value;
}