/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMseq.h"

RFSMseq::RFSMseq(std::string file, std::string blockpath, std::string blockname, int line)
	:RFSMStmtBlock(CLS_RFSMseqStmt, file, blockpath, blockname, 10, line)
{
}

RFSMseq:: ~RFSMseq(void)
{	
}

std::string RFSMseq::toString()
{
	std::string str = "sequencial";
	str.append(RFSMStmtBlock::toString());

	return str;
}
