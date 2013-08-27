/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMgotoStmt.h"

RFSMgotoStmt::RFSMgotoStmt(std::string name, std::string file, std::string blockpath, std::string blockname, int line) 
	:RFSMstmt(CLS_RFSMgotoStmt, file, blockpath, blockname, line), 
	 RFSMnamedItem(name)
{
}

RFSMgotoStmt:: ~RFSMgotoStmt(void)
{
}

std::string RFSMgotoStmt::toString()
{
	return this->getName();
}
