/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/symtab/TABenum.h"

TABenum::TABenum(std::string typeName, std::string enumName, unsigned int value):RFSMnamedItem(enumName)
{
	m_typeName = typeName;
	m_value = value;
}

TABenum:: ~TABenum(void)
{
}

void TABenum::setType(unsigned int type)
{
	m_type = type;
}

unsigned int TABenum::getValue()
{
	return m_value;
}

unsigned int TABenum::getType()
{
	return m_type;
}

std::string TABenum::getTypeName()
{
	return m_typeName;
}