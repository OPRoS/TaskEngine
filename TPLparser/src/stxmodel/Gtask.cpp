/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/Gtask.h"

Gtask::Gtask(std::string file, int eol, int blockid, std::string blockname, std::string blockpath): GcstBlock(file, eol, blockid, blockname, blockpath)
{
	m_runBlock = NULL;
}

Gtask:: ~Gtask(void)
{
	if(m_runBlock != NULL) delete m_runBlock;
}

void Gtask::setRunBlock(GstmtBlock* rBlock)
{
	m_runBlock = rBlock;
}

GstmtBlock* Gtask::getRunBlock()
{
	return m_runBlock;
}

void Gtask::print(unsigned int ident)
{
	titlePrint(ident, std::string("TASK"));
	print(ident +1);
}

