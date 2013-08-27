/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2011.08.16
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "scenario/NodeCon.h"

NodeCon::NodeCon(int id): NodeBS(id)
{
}

NodeCon::~NodeCon(void)
{
	if(m_childnodes.size()>0){
		for(unsigned int i=0; i<m_childnodes.size();i++){
			delete m_childnodes[i];
		}
	}
}

void NodeCon::addSub(NodeBS* bhv)
{
	m_childnodes.push_back(bhv);
}

void NodeCon::setJoinType(unsigned int type)
{
	m_jtype = type;
}

std::vector<NodeBS*> NodeCon::getSubNodes()
{
	return m_childnodes;
}

unsigned int NodeCon::getJoinType()
{
	return m_jtype;
}