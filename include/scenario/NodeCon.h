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
#include <vector>

class NodeCon : public NodeBS
{
private:	
	std::vector<NodeBS*>	m_childnodes;
	unsigned int			m_jtype;

public:
	NodeCon(int);
	virtual ~NodeCon(void);

	void addSub(NodeBS*);
	void setJoinType(unsigned int type);

	std::vector<NodeBS*> getSubNodes(void);
	unsigned int getJoinType();
};