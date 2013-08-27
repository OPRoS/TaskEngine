/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2011.08.16
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "scenario/NodeState.h"

NodeState::NodeState(std::string cond, std::string bhv, std::string prefix): NodeBS(-1)
{
	this->setEvtRef(cond);
	this->setPrefix(prefix);
	m_bhv = bhv;
	m_isInitial = false;
}

//NodeState::NodeState(NodeBS* node): NodeBS(node->getNodeId())
//{
//}

NodeState::~NodeState(void)
{
	if(m_trans.size()>0){
		for(unsigned int i=0; i<m_trans.size();i++){
			delete m_trans[i];
		}
	}
}

int NodeState::addTrans(NodeTrans* trans)
{
	if(m_trans.size()>0){
		for(unsigned int i=0; i<m_trans.size();i++){
			if(trans->getCond() == m_trans[i]->getCond() &&
				trans->getStateAfterExpand() == m_trans[i]->getStateAfterExpand() &&
				trans->getTransSource() == m_trans[i]->getTransSource() &&
				trans->getTransTarget() == m_trans[i]->getTransTarget() &&
				trans->getTransType() == m_trans[i]->getTransType()){
				return -1;
			}
		}
	}
	m_trans.push_back(trans);
	return 0;
}

std::vector<NodeTrans*> NodeState::getTrans()
{
	return m_trans;
}

std::string NodeState::getBhvName()
{
	return m_bhv;
}

std::string NodeState::getEvtBhvStrInPrefix()
{
	std::string evt		= this->getEvtRef();
	std::string prefix	= this->getPrefix();

	std::string str;
	if(evt != "") str.append("_").append(evt);
	str.append("_").append(m_bhv);

	return str;
}

bool NodeState::isStartState(std::string fsmprefix)
{
	std::string str = getAheadStatePrefix();
	
	if(str == fsmprefix)
		return true;
	else
		return false;
}

std::string NodeState::getAheadStatePrefix()
{
	std::string prefix	= this->getPrefix();
	std::string str = this->getEvtBhvStrInPrefix();
	int s = str.size();
	std::string ap = prefix.substr(0, prefix.size()-str.size());

	return ap;
}

bool NodeState::isAmbiguous()
{
	//A->e1:B, A->e2:C에서 A에 조건이 있는 것이 아니다.
	int numOfwithoutCond;
	if(m_trans.size()>1){
		for(unsigned int i=0; i<m_trans.size();i++){
			numOfwithoutCond = -1; //자신과 비교를 카운더하지 않기 위해서 -1
			std::string cond1 = m_trans[i]->getCond();
			for(unsigned int j=0; j<m_trans.size();j++){
				std::string cond2 = m_trans[j]->getCond();
				if(cond1 == cond2) numOfwithoutCond++;
			}
			//if(m_trans[i]->getCond() == "")
				//numOfwithoutCond++;
			if(numOfwithoutCond>=1) return true;
		}
	
	}
	return false;
}
void NodeState::deleteAllTrans()
{
	for(unsigned int i=0; i<m_trans.size();i++){
		delete m_trans[i];
	}
	m_trans.clear();
}

bool NodeState::isInitial()
{
	return m_isInitial;
}

void NodeState::setAsInitial(bool isInitial)
{
	m_isInitial = isInitial;
}

/*
std::string NodeState::getMonOp(void)
{
	return m_monOp;
}

std::string NodeState::getMonEvt(void)
{
	return m_monEvt;
}

void NodeState::setMonOp(std::string monOp)
{
	m_monOp = monOp;
}

void NodeState::setMonEvt(std::string monEvt)
{
	m_monEvt = monEvt;
}
*/