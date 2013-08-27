/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/Gsynch.h"

Gsynch::Gsynch(std::string file, int eol, std::string blockname, std::string blockpath):Gstmt(STMT_SYNCH, file, eol, blockname, blockpath)
{
	m_name = NULL;
	m_time = NULL;
}

Gsynch:: ~Gsynch(void)
{
	if(m_name != NULL)
		delete m_name;	
	if(m_time != NULL)
		delete m_time;
}

void Gsynch::setSynchID(GstmtExpr* time)
{
	m_time = time;
}

Gtoken* Gsynch::getName()
{
	return m_name;
}

GstmtExpr* Gsynch::getSynchID()
{
	return m_time;
}