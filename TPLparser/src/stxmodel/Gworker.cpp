/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/Gworker.h"

Gworker::Gworker(std::string file, int eol, int blockid, std::string blockname, std::string blockpath) : GstmtBlock(file, eol, blockid, blockname, blockpath)
{
	m_name = NULL;
	m_desc = NULL;
	m_startBhv = NULL;
}

Gworker:: ~Gworker(void)
{
	if(m_name != NULL) delete m_name;
	if(m_desc != NULL) delete m_desc;
	if(m_startBhv != NULL) delete m_startBhv;
}

void Gworker::setName(Gtoken *name)
{
	m_name = name;
}

void Gworker::setDesc(Gtoken *desc)
{
	m_desc = desc;
}

void Gworker::setStartBhv(Gtoken *startBhvName)
{
	m_startBhv = startBhvName;
}

Gtoken* Gworker::getName()
{
	return m_name;
}

Gtoken* Gworker::getDesc()
{
	return m_desc;
}

Gtoken* Gworker::getStartBhv()
{
	return m_startBhv;
}

void Gworker::print(unsigned int ident)
{
	titlePrint(ident, std::string("TASK"));
	print(ident +1);
}

