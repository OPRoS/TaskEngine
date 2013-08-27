/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMbehavior.h"

RFSMbehavior::RFSMbehavior(std::string name, int nameLine, int endline, std::string blockID, std::string file, bool isMission): RFSMtreenode(name, nameLine, endline, blockID, file, isMission)
{
	m_states = NULL;
}

RFSMbehavior:: ~RFSMbehavior(void)
{
	if(m_states != NULL){
		for(unsigned int i=0 ; i<m_states->size(); i++){
			if((*m_states)[i] != NULL) delete (*m_states)[i];
		}
		//m_states.clear();
		//while(m_states->moveHead()){
		//	RFSMstate* obj = m_states->getCurObject();
		//	delete obj;
		//	m_states->delHead();
		//}
		delete m_states;
	}	
}

void RFSMbehavior::setStates(std::vector<RFSMstate*>* states)
{
	if(m_states != NULL){
		for(unsigned int i=0 ; i<m_states->size(); i++){
			if((*m_states)[i] != NULL) delete (*m_states)[i];
		}
		//m_states.clear();
		//while(m_states->moveHead()){
		//	RFSMstate* obj = m_states->getCurObject();
		//	delete obj;
		//	m_states->delHead();
		//}
		delete m_states;
	}	

	m_states = states;
}

std::string RFSMbehavior::getInitialStateName()
{
/*
	if(m_states != NULL && m_states->getSize()>0 && m_states->moveHead()){
		do{
			RFSMstate* state = m_states->getCurObject();
			int mod = state->getModifier();
			if(mod == INITIAL_STATE){
				return state->getName();
			}
		}
		while(m_states->moveNext());
	}
*/
	if(m_states != NULL){
		for(unsigned int i=0 ; i<m_states->size(); i++){
			if((*m_states)[i]->getModifier() == INITIAL_STATE) 
				return (*m_states)[i]->getName();
		}
	}	

	return "";
}

std::vector<std::string> RFSMbehavior::getGoalStateNames()
{
	/*
	if(m_states != NULL && m_states->getSize()>0 && m_states->moveHead()){
		do{
			RFSMstate* state = m_states->getCurObject();
			int mod = state->getModifier();
			if(mod == GOAL_STATE){
				return state->getName();
			}
		}
		while(m_states->moveNext());
	}
	return "";
	*/
	std::vector<std::string> gs;
	if(m_states != NULL){
		for(unsigned int i=0 ; i<m_states->size(); i++){
			if((*m_states)[i] != NULL && (*m_states)[i]->getModifier() == GOAL_STATE) 
				gs.push_back((*m_states)[i]->getName());
		}
	}	

	return gs;
}

RFSMstate* RFSMbehavior::getInitialState()
{
	/*
	if(m_states != NULL && m_states->getSize()>0 && m_states->moveHead()){
		do{
			RFSMstate* state = m_states->getCurObject();
			int mod = state->getModifier();
			if(mod == INITIAL_STATE){
				return state;
			}
		}
		while(m_states->moveNext());
	}
	*/

	if(m_states != NULL){
		for(unsigned int i=0 ; i<m_states->size(); i++){
			if((*m_states)[i]->getModifier() == INITIAL_STATE) 
				return (*m_states)[i];
		}
	}	
	return NULL;
}

std::vector<RFSMstate*> RFSMbehavior::getGoalStates()
{
	/*
	if(m_states != NULL && m_states->getSize()>0 && m_states->moveHead()){
		do{
			RFSMstate* state = m_states->getCurObject();
			int mod = state->getModifier();
			if(mod == GOAL_STATE){
				return state;
			}
		}
		while(m_states->moveNext());
	}
	return NULL;
	*/

	std::vector<RFSMstate*> gs;
	if(m_states != NULL){
		for(unsigned int i=0 ; i<m_states->size(); i++){
			if((*m_states)[i] != NULL && (*m_states)[i]->getModifier() == GOAL_STATE) 
				gs.push_back((*m_states)[i]);
		}
	}	
	return gs;
}

RFSMstate* RFSMbehavior::getState(std::string stateName)
{
	/*if(m_states != NULL && m_states->getSize()>0 && m_states->moveHead()){
		do{
			RFSMstate* state = m_states->getCurObject();
			std::string name = state->getName();
			if(name.compare(stateName)==0){
				return state;
			}
		}
		while(m_states->moveNext());
	}
	return NULL;*/

	std::string name;
	if(m_states != NULL){
		for(unsigned int i=0 ; i<m_states->size(); i++){
			if((*m_states)[i] != NULL){
				name = (*m_states)[i]->getName();
				if(name.compare(stateName)==0) 
					return (*m_states)[i];
			}
		}
	}	
	return NULL;
}

std::vector<RFSMstate*>* RFSMbehavior::getAllStates()
{
	return m_states;
}

std::string RFSMbehavior::getDot()
{
	std::string clusterPrefix = "cluster";
	std::string dot;
	dot.append("subgraph ");
	dot.append(clusterPrefix);
	dot.append(m_name);
	dot.append("{\n");
	dot.append("label=\"");dot.append(m_name);dot.append("\";\n");

	if(m_states != NULL){
		for(unsigned int i=0 ; i<m_states->size(); i++){
			
			//bhv에 포함된 state의 이름 각각에 대해서
			RFSMstate* state = (*m_states)[i];
			std::string stateN = RFSMnamedItem::removePath(state->getName());

			//->와 목적 state를 적는다.
			std::vector<std::string>* linkedS = state->getLinkedStates();
			if(linkedS != NULL){
				for(unsigned int i=0 ; i<linkedS->size(); i++){
					dot.append(stateN);
					dot.append("->");
					std::string gotoS = RFSMnamedItem::removePath((*linkedS)[i]);
					if(gotoS.compare("stay")==0)
						dot.append(stateN);
					else dot.append(gotoS);
					dot.append(";\n");
				}
			}

			std::vector<std::string>* linkedT = state->getLinkedTasks();
			if(linkedT != NULL){
				for(unsigned int i=0 ; i<linkedT->size(); i++){
					dot.append(stateN);
					dot.append("->");
					//%taskname*
					std::string gotoT = "%";
					gotoT.append((*linkedT)[i]);
					gotoT.append("*");
					dot.append(gotoT);
					dot.append("[lhead = ");
					dot.append(clusterPrefix);
					dot.append((*linkedT)[i]);
					dot.append("]");
					dot.append(";\n");
				}
			}
			
		}
	}

	dot.append("}\n");
	return dot;
}
