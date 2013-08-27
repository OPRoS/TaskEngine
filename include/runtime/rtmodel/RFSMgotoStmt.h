/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "runtime/rtmodel/RFSMstmt.h"
#include "runtime/rtmodel/RFSMnamedItem.h"

class RFSMgotoStmt : public RFSMstmt, public RFSMnamedItem
{
public:
	RFSMgotoStmt(std::string, std::string file, std::string blockpath, std::string blockname, int line);
	virtual ~RFSMgotoStmt(void);

	std::string toString();
};
