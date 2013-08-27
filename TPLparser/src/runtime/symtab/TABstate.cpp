/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/symtab/TABstate.h"

TABstate::TABstate(std::string taskName, std::string name):RFSMnamedItem(name)
{
	m_taskName = taskName;
}

TABstate:: ~TABstate(void)
{
}


std::string TABstate::getTaskName()
{
	return m_taskName;
}