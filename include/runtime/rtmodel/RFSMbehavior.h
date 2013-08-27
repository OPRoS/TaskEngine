/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include "runtime/rtmodel/RFSMvalue.h"
#include "runtime/rtmodel/RFSMstate.h"
#include "runtime/rtmodel/RFSMtransition.h"
#include "runtime/rtmodel/RFSMnamedItem.h"
#include "runtime/rtmodel/RFSMBreakPoint.h"
#include "runtime/rtmodel/RFSMtreenode.h"

class RFSMbehavior : public RFSMtreenode
{
private:
	//EList<RFSMstate*>*	m_states;
	std::vector<RFSMstate*>* m_states;

public:
	RFSMbehavior(std::string name, int nameLine, int endline, std::string blockID, std::string file, bool isMission);
	virtual ~RFSMbehavior(void);
	
	void				setStates			(std::vector<RFSMstate*>*);
		
	std::string			getInitialStateName	(void);
	RFSMstate*			getInitialState		(void);
	RFSMstate*			getState			(std::string stateName);

	std::vector<RFSMstate*>		getGoalStates		(void);
	std::vector<std::string>	getGoalStateNames	(void);
	std::vector<RFSMstate*>*	getAllStates		(void);
	
	std::string			getDot();
};