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
#include "scenario/NodeBS.h"
#include "scenario/NodeBhv.h"
#include "scenario/NodeSeq.h"

class SBhvSeq : public SObj
{
private:
	NodeBS*		m_bs;	
	std::string	m_goto;

public:
	SBhvSeq(void);
	virtual ~SBhvSeq(void);

	void setStartNode(NodeBS*);
	NodeBS*	getStartNode(void);

	void setGoto(std::string);
	std::string	getGoto(void);

	void makeGotoBhv(std::string);
};
