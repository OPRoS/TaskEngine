/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMstate.h"

RFSMstate::RFSMstate(std::string path, std::string name, int nameLine, std::string blockid, std::string file, int line)
	: RFSMcst(name, nameLine, blockid), 
	  /*RFSMBreakPoint(file)*/
	  RFSMBlockInfo(path, name, file, line)
{
	m_transition	= NULL;
	m_dos			= NULL;
}

RFSMstate:: ~RFSMstate(void)
{
	if(m_dos != NULL){
		/*
		while(m_dos->moveHead()){
			RFSMstmt* obj = m_dos->getCurObject();
			delete obj;
			m_dos->delHead();
		}
		*/
		for(unsigned int i=0 ; i<m_dos->size(); i++){
			delete (*m_dos)[i];
		}
		delete m_dos;
		
	}
	if(m_transition != NULL){
		/*while(m_transition->moveHead()){
			RFSMstmt* obj = m_transition->getCurObject();
			delete obj;
			m_transition->delHead();
		}*/
		for(unsigned int i=0 ; i<m_transition->size(); i++){
			delete (*m_transition)[i];
		}
		delete m_transition;
	}
	m_linkedStates.clear();
	m_linkedTasks.clear();

	/*while(m_linkedStates.moveHead()){			
		m_linkedStates.delHead();
	}
	
	while(m_linkedTasks.moveHead()){			
		m_linkedTasks.delHead();
	}*/
	
}

void RFSMstate::setModifier(int mod)
{
	m_mod = mod;
}

void RFSMstate::setTransition(std::vector<RFSMstmt*>* transBlock)
{
	m_transition = transBlock;
}

void RFSMstate::addLinkedstate(std::string linkedstate)
{
	m_linkedStates.push_back(linkedstate);
}

void RFSMstate::addLinkedtask(std::string linkedtask)
{
	m_linkedTasks.push_back(linkedtask);
}

std::vector<std::string>* RFSMstate::getLinkedStates()
{
	return &m_linkedStates;
}

std::vector<std::string>* RFSMstate::getLinkedTasks()
{
	return &m_linkedTasks;
}

void RFSMstate::setDostmts(std::vector<RFSMstmt*>* dos)
{
	m_dos = dos;
}

std::vector<RFSMstmt*>* RFSMstate::getTransition()
{
	return m_transition;
}

std::vector<RFSMstmt*>* RFSMstate::getDostmts()
{
	return m_dos;
}

int RFSMstate::getModifier()
{
	return m_mod;
}

bool RFSMstate::isGoalState()
{
	if(m_mod == GOAL_STATE)
		return true;
	return false;
}

int	RFSMstate::getEOLtrans()
{
	return m_EOL_trans;
}

int	RFSMstate::getEOLstay()
{
	return m_EOL_stay;
}

void RFSMstate::setEOLtrans(int eol)
{
	m_EOL_trans = eol;
}

void RFSMstate::setEOLstay(int eol)
{
	m_EOL_stay= eol;
}