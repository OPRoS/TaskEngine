/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Scenario 
** Date:        2011.08.15
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "scenario/NodeTrans.h"

NodeTrans::NodeTrans(void)
{
}

NodeTrans::~NodeTrans(void)
{
}

void NodeTrans::setTransSource(std::string sname)
{
	m_sourceName = sname;
}

void NodeTrans::setCond(std::string evt)
{
	m_cond = evt;
}

void NodeTrans::setTransTarget(unsigned int type, std::string tname)
{
	m_TargetType = type;
	m_targetName = tname;
}

void NodeTrans::setStateAfterExpand(std::string nxt)
{
	m_nextState = nxt;
}

void NodeTrans::setMonPrefix(std::string monPrefix)
{
	m_monPrefix = monPrefix;
}

unsigned int NodeTrans::getTransType()
{
	return m_TargetType;
}

std::string NodeTrans::getTransTarget()
{
	return m_targetName;
}

std::string NodeTrans::getTransSource()
{
	return m_sourceName;
}

std::string NodeTrans::getStateAfterExpand()
{
	return m_nextState;
}

std::string NodeTrans::getCond()
{
	return m_cond;
}

std::string NodeTrans::getMonPrefix()
{
	return m_monPrefix;
}


