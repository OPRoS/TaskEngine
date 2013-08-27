/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMparellel.h"

RFSMparellel::RFSMparellel(std::string file, std::string blockpath, std::string blockname, int line)
	: RFSMStmtBlock(CLS_RFSMparStmt, file, blockpath, blockname, 10, line)
{
}

RFSMparellel:: ~RFSMparellel(void)
{	
}

std::string RFSMparellel::toString()
{
	std::string str = "parellel";
	str.append(RFSMStmtBlock::toString());

	return str;
}
