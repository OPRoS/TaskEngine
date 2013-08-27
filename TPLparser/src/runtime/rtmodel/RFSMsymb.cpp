/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMsymb.h"

RFSMsymb::RFSMsymb(std::string sName, std::string file, std::string blockpath, std::string blockname, int line)
	:RFSMstmt(CLS_RFSMsymbStmt, file, blockpath, blockname, line)
{	
	m_symName = sName;
}

RFSMsymb:: ~RFSMsymb(void)
{
}

std::string RFSMsymb::toString()
{
	return m_symName;
}
