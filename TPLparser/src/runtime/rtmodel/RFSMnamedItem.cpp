/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMnamedItem.h"

RFSMnamedItem::RFSMnamedItem(std::string name)
{
	m_name = name;
}

RFSMnamedItem::RFSMnamedItem(char* name)
{
	m_name = std::string(name);
}

RFSMnamedItem:: ~RFSMnamedItem(void)
{
}

void RFSMnamedItem::appendName(std::string name)
{
	m_name.append(name);
}

bool RFSMnamedItem::isSameName(std::string name)
{
	if(m_name.compare(name) == 0)
		return true;
	else
		return false;
}

std::string RFSMnamedItem::getName()
{
	return m_name;
}


std::string RFSMnamedItem::removePath(std::string name)
{
	std::vector<std::string> path = Estring::split(name, ".");

	std::vector<std::string>::const_iterator i;

	return (path.at(path.size()-1));
}

void RFSMnamedItem::setName(std::string name)
{
	m_name = name;
}
