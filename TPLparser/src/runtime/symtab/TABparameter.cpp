/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/symtab/TABparameter.h"

TABparameter::TABparameter(std::string type, std::string name)
{
	m_type = type;
	m_name = name;
}

TABparameter:: ~TABparameter()
{
}

std::string	TABparameter::getType()
{
	return m_type;
}

std::string	TABparameter::getName()
{
	return m_name;
}
