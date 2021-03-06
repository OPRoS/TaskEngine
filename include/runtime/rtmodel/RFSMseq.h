/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "runtime/rtmodel/RFSMStmtBlock.h"

class RFSMseq : public RFSMStmtBlock
{

public:
	RFSMseq(std::string file, std::string blockpath, std::string blockname, int line);
	virtual ~RFSMseq(void);
	
	std::string toString();
};