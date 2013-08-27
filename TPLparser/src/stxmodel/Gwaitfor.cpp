/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/Gwaitfor.h"

Gwaitfor::Gwaitfor(std::string file, int eol, std::string blockname, std::string blockpath):Gstmt(STMT_SLEEP, file, eol, blockname, blockpath)
{
	m_name = NULL;
	m_time = NULL;
}

Gwaitfor:: ~Gwaitfor(void)
{
	if(m_name != NULL)
		delete m_name;	
	if(m_time != NULL)
		delete m_time;
}

void Gwaitfor::setTime(GstmtExpr* time)
{
	m_time = time;
}

Gtoken* Gwaitfor::getName()
{
	return m_name;
}

GstmtExpr* Gwaitfor::getTime()
{
	return m_time;
}