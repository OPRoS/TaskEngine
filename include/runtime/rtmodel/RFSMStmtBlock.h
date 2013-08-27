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
#include "runtime/rtmodel/RFSMboolable.h"

class RFSMStmtBlock : public RFSMstmt
{
private:	
	std::vector<RFSMstmt*>*	m_stmts;

public:	
	RFSMStmtBlock(std::string, std::string file, std::string blockpath, std::string blockname, int period, int line);
	virtual ~RFSMStmtBlock(void);

	void setStmtBlock(std::vector<RFSMstmt*>*);
	std::vector<RFSMstmt*>*	getStmtBlock();
	
	std::string toString();

};