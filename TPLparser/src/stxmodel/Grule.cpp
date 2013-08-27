/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/Grule.h"

Grule::Grule(std::string file, int eol, std::string blockname, std::string blockpath):Gcondition(file, eol, blockname, blockpath)
{
	// TODO Auto-generated constructor stub

}

Grule:: ~Grule()
{
	// TODO Auto-generated destructor stub
}

void Grule::setName(Gtoken* name)
{
	m_name = name;
}

void Grule::print(unsigned int indent)
{
	titlePrint(indent, std::string("RULE"));
	if(m_name!=NULL) m_name->print(indent+1);
	Gcondition::print(indent+1);
}
