/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2011.08.16
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "scenario/NodeBhv.h"

NodeBhv::NodeBhv(int nodeid): NodeBS(nodeid)
{
	m_isCycled = false;
	m_sub = NULL;
	m_bhv = NULL;
}

NodeBhv::~NodeBhv(void)
{
	if(m_bhv != NULL) delete m_bhv;
}

void NodeBhv::setSBehavior(SBehavior* bhv)
{
	m_bhv = bhv;
}
void NodeBhv::setBhvRef(std::string bhvRef)
{
	m_bhvRef = bhvRef;
}

void NodeBhv::setSub(NodeBS* sub)
{
	m_sub = sub;
}

void NodeBhv::setAsCycled(void)
{
	m_isCycled = true;
}

SBehavior* NodeBhv::getSBehavior()
{
	return m_bhv;
}

std::string	 NodeBhv::getBhvRef()
{
	return m_bhvRef;
}

NodeBS* NodeBhv::getSubNode()
{
	return m_sub;
}

bool NodeBhv::isCycled(void)
{
	return m_isCycled;
}


void NodeBhv::setSubMonOp(std::string op)
{
	m_subMonOp = op;
}

void NodeBhv::setSubMonEvt(std::string evt)
{
	m_subMonEvt = evt;
}
		
std::string	NodeBhv::getSubMonOp(void)
{
	return m_subMonOp;
}

std::string NodeBhv::getSubMonEvt(void)
{
	return m_subMonEvt;
}