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

class RFSMifStmt : public RFSMstmt
{
private:
	RFSMboolable*			m_cond ;
	std::vector<RFSMstmt*>*	m_Tstmts;
	std::vector<RFSMstmt*>*	m_Fstmts;

public:
	RFSMifStmt(std::string, std::string blockpath, std::string blockname, int line);
	virtual ~RFSMifStmt(void);

	void setCondition	(RFSMboolable*);
	void setTrueStmts	(std::vector<RFSMstmt*>*);
	void setFalseStmts	(std::vector<RFSMstmt*>*);

	RFSMboolable*			getCondition	(void);
	std::vector<RFSMstmt*>*	getTrueStmts	(void);
	std::vector<RFSMstmt*>*	getFalseStmts	(void);

	std::string toString();
};
