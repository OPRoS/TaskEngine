/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/Gstmt.h"

Gstmt::Gstmt(std::string stmtName, std::string fname, int eol, std::string blockname, std::string blockpath):Ggram(fname, eol, blockname, blockpath)
{
	m_stmtName = stmtName;
}

Gstmt::Gstmt(std::string stmtName, std::string fname):Ggram(fname)
{
	m_stmtName = stmtName;
}

Gstmt:: ~Gstmt(void)
{
}


std::string Gstmt::getStmtName()
{
	return m_stmtName;
}