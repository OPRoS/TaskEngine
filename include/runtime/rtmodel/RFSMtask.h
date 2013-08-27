/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include <string>
#include <set>
#include "runtime/rtmodel/RFSMvalue.h"
#include "runtime/rtmodel/RFSMcst.h"
#include "runtime/rtmodel/RFSMBreakPoint.h"
#include "runtime/rtmodel/RFSMifStmt.h"
#include "runtime/rtmodel/RFSMcallConnector.h"
#include "runtime/rtmodel/RFSMcallBhv.h"
#include "runtime/rtmodel/RFSMStmtBlock.h"
#include "runtime/rtmodel/RFSMtreenode.h"

class RFSMtask : public RFSMtreenode
{
private:
	int						m_EOLrun;
	std::set<int>			m_synchidInTask;
	std::vector<RFSMstmt*>*	m_runBlock;
	
public:
	RFSMtask(std::string name, int nameLine, int endline, std::string blockID, std::string);
	virtual ~RFSMtask(void);

	void setSynchidInTask	(void);
	void setRunBlock		(std::vector<RFSMstmt*>*);	
	void extractStartNodes	(std::vector<RFSMstmt*>*, std::vector<std::string>&);

	int		getEOLrun	(void);
	void	setEOLrun	(int);

	std::vector<RFSMstmt*>*	getRunBlock		(void);
	std::set<int>			getTaskSynchIds	(void);

};
