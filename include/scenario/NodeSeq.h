/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Scenario 
** Date:        2011.08.15
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "scenario/NodeBS.h"

class NodeSeq : public NodeBS
{
private:	
	std::vector<NodeBS*> m_childnodes;

public:
	NodeSeq(int);
	virtual ~NodeSeq(void);

	void addSub(NodeBS*);
	std::vector<NodeBS*> getSubNodes(void);
};