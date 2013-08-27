/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2011.08.16
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "scenario/FSM4BS.h"

FSM4BS::FSM4BS(std::string prefix, std::string refBhv, std::string name, std::string type)
{
	m_name		= name;
	m_refBhv	= refBhv;
	m_prefix	= prefix;
	m_type		= type;
	m_isMonitorSelector = false;
}

FSM4BS::~FSM4BS(void)
{
	if(m_states.size()>0){
		for(unsigned int i=0; i<m_states.size();i++){
			delete m_states[i];
		}
	}
}

void FSM4BS::addBhvSeq(NodeBS* bs)
{
	m_bhvSeqs.push_back(bs);
}

std::vector<NodeBS*> FSM4BS::getBhvSeqList()
{
	return m_bhvSeqs;
}

std::string FSM4BS::getName()
{
	return m_name;
}

std::string FSM4BS::getPrefix()
{
	return m_prefix;
}


std::string FSM4BS::getRefBhv()
{
	return m_refBhv;
}

void FSM4BS::addState(NodeState* state)
{
	for(unsigned int i=0; i<m_states.size();i++){
		if(m_states[i]->getState()==state->getState())
			return;
	}
	m_states.push_back(state);
}

void FSM4BS::setMonOp(std::string monType)
{
	m_monType = monType;
}

void FSM4BS::setMonEvt(std::string monEvt)
{
	m_monEvt = monEvt;
}

void FSM4BS::setPrefix4MonTarget(std::string prefix4monTarget)
{
	m_prefix4monTarget = prefix4monTarget;
}

std::vector<NodeState*> FSM4BS::getStates()
{
	return m_states;
}

NodeState* FSM4BS::getState(std::string state)
{
	for(unsigned int i=0; i<m_states.size();i++){
		if(m_states[i]->getState()==state)
			return m_states[i];
	}
	return NULL;
}

std::string FSM4BS::getInitialState(void)
{
	for(unsigned int i=0; i<m_states.size();i++){
		if(m_states[i]->isInitial())
			return m_states[i]->getState();
	}
	return "";
}

NodeState* FSM4BS::getLastState()
{
	if(m_states.size()>0){
		return m_states[m_states.size()-1];
	}
	return NULL;
}

std::string FSM4BS::getType()
{
	return m_type;
}

std::string FSM4BS::getMonOp()
{
	return m_monType;
}

std::string FSM4BS::getMonEvt()
{
	return m_monEvt;
}

std::string FSM4BS::getPrefix4MonTarget()
{
	return m_prefix4monTarget;
}

void FSM4BS::setAsMonitorSelector()
{
	m_isMonitorSelector = true;
}

bool FSM4BS::isMonitorSelector()
{
	return m_isMonitorSelector;
}
/*
void FSM4BS::addTrans(NodeTrans* state)
{
	m_trans.push_back(state);
}

std::vector<NodeTrans*> FSM4BS::getTrans()
{
	return m_trans;
}
*/