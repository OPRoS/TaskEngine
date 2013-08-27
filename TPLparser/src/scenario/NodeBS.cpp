/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2011.08.16
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "scenario/NodeBS.h"

NodeBS::NodeBS(int nodeid)
{
	m_id = nodeid;
	//m_cond = NULL;
}

NodeBS::~NodeBS(void)
{
	//if(m_cond != NULL) delete m_cond;
}

void NodeBS::setEvtRef(std::string evtRef)
{
	m_evtRef=evtRef;
}

//void NodeBS::setUname(std::string uname)
//{
//	m_uname = uname;
//}

void NodeBS::setState(std::string state)
{
	m_state = state;
}

void NodeBS::setPrefix(std::string prefix)
{
	m_prefix = prefix;
}

void NodeBS::addExpandStr(std::string expand)
{
	m_expandlist.push_back(expand);
}

std::string NodeBS::getEvtRef()
{
	return m_evtRef;
}

int NodeBS::getNodeId()
{
	return m_id;
}

std::string NodeBS::getPrefix()
{
	return m_prefix;
}

//std::string NodeBS::getUname()
//{
//	return m_uname;
//}

std::string NodeBS::getState()
{
	return m_state;
}