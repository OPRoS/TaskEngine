/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Scenario 
** Date:        2011.08.15
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "scenario/NodeTrans.h"
#include "scenario/NodeBS.h"
#include <string>
#include <vector>

class NodeState : public NodeBS
{
private:	
	std::vector<NodeTrans*>	m_trans;
	std::string m_bhv;
	bool m_isInitial;

	std::string m_monOp;
	std::string m_monEvt;

public:
	NodeState(std::string, std::string, std::string);
	NodeState(NodeBS*);
	virtual ~NodeState(void);

	std::vector<NodeTrans*>	getTrans	(void);
	std::string	getAheadStatePrefix		(void);
	std::string getEvtBhvStrInPrefix	(void);
	std::string getBhvName				(void);
	//std::string getMonOp				(void);
	//std::string getMonEvt				(void);

	bool isStartState	(std::string);
	void deleteAllTrans	(void);
	bool isAmbiguous	(void);
	int  addTrans		(NodeTrans*);
	bool isInitial		(void);
	void setAsInitial	(bool);
	//void setMonOp		(std::string);
	//void setMonEvt		(std::string);

};