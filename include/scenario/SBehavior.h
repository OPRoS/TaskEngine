/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Scenario 
** Date:        2011.08.15
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "scenario/SObj.h"
#include "stxmodel/GstmtBlock.h"
#include "ds/EList.h"

class SBehavior : public SObj
{
private:
	EList<Gstmt*>*	m_stmtList;	
	std::string		m_stmtsStr;

public:
	SBehavior(void);
	virtual ~SBehavior(void);

	void setStmtList(EList<Gstmt*>*);
	void setStmtsStr(std::string);

	EList<Gstmt*>*	getStmtList();
	std::string		getStmtStr();
};
