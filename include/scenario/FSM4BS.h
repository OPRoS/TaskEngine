/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Scenario 
** Date:        2011.09.12
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "scenario/NodeState.h"

class FSM4BS
{
private:
	std::vector<NodeState*>	m_states;
	//std::vector<NodeTrans*>	m_trans;
	std::string				m_name;   //ex) B_MON
	std::string				m_refBhv; //ex) B
	std::vector<NodeBS*>	m_bhvSeqs;
	std::string				m_prefix;
	std::string				m_type; //scenario ¶Ç´Â subtask
	std::string				m_monType; // ~, ^, *
	std::string				m_monEvt;
	std::string				m_prefix4monTarget;
	bool					m_isMonitorSelector;

public:
	FSM4BS(std::string, std::string, std::string, std::string);
	virtual ~FSM4BS(void);

	void addState	(NodeState*);
	void addBhvSeq	(NodeBS*);
	void setMonOp	(std::string);
	void setMonEvt	(std::string);
	void setPrefix4MonTarget (std::string);
	void setAsMonitorSelector();
	//void addTrans(NodeTrans*);	
	std::vector<NodeState*> getStates		(void);
	std::vector<NodeBS*>	getBhvSeqList	(void);
	//std::vector<NodeTrans*>	getTrans(void);
	
	std::string	getRefBhv	(void);
	std::string getName		(void);
	std::string getPrefix	(void);
	NodeState*	getLastState(void);
	NodeState*	getState	(std::string);
	std::string getInitialState(void);
	std::string getType		(void);
	std::string getMonOp	(void);
	std::string getMonEvt	(void);
	std::string getPrefix4MonTarget (void);

	bool isMonitorSelector();
};
