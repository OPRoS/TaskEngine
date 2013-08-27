/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Scenario 
** Date:        2011.08.15
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "stxmodel/Gcondition.h"
#include "scenario/SObj.h"

class SEvent : public SObj
{
private:
	GstmtExpr*	m_cond;	
	std::string m_condStr;

public:
	SEvent(void);
	virtual ~SEvent(void);

	void setCondition(GstmtExpr*);
	void setCondStr(std::string);

	GstmtExpr*	getCondition();
	std::string	getCondStr();
};
