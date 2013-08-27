/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2011.08.16
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "scenario/NodeSeq.h"

NodeSeq::NodeSeq(int id): NodeBS(id)
{
}

NodeSeq::~NodeSeq(void)
{
	if(m_childnodes.size()>0){
		for(unsigned int i=0; i<m_childnodes.size();i++){
			delete m_childnodes[i];
		}
	}
}

void NodeSeq::addSub(NodeBS* bhv)
{
	m_childnodes.push_back(bhv);
}

std::vector<NodeBS*> NodeSeq::getSubNodes()
{
	return m_childnodes;
}

