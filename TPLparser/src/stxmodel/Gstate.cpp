/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/Gstate.h"

/*
Gstate::Gstate(std::string file, int eol, int blockid, std::string blockname, std::string blockpath) : GstmtBlock(file, eol, blockid, blockname, blockpath)
{	
	m_name			= NULL;
	m_transition	= NULL;
	m_do			= NULL;
	m_entry			= NULL;
	m_exit			= NULL;
	//m_stateVarList	= NULL;
}
*/
Gstate::Gstate(std::string file,  int blockid):GstmtBlock(file, blockid)
{
	m_name			= NULL;
	m_transition	= NULL;
	m_do			= NULL;
	m_entry			= NULL;
	m_exit			= NULL;
}

Gstate:: ~Gstate(void)
{
	if(m_name != NULL)
		delete m_name;
	if(m_transition != NULL)
		delete m_transition;
	if(m_do != NULL)
		delete m_do;
	if(m_entry != NULL)
		delete m_entry;
	if(m_exit != NULL)
		delete m_exit;
	//if(m_stateVarList != NULL)
	//	delete m_stateVarList;
}

void Gstate::setModifier(int mod)
{
	m_mod = mod;
}

void Gstate::setName(Gtoken *name)
{
	m_name = name;
}

void Gstate::setTransition(GstateTrans *transition)
{
	if(m_transition != NULL)
		delete m_transition;

	m_transition = transition;
}

void Gstate::setDo(GstateDo* doaction)
{
	if(m_do != NULL)
		delete m_do;

	m_do = doaction;
}

void Gstate::setEntry(GstateEntry* entry)
{
	if(m_entry != NULL)
		delete m_entry;

	m_entry = entry;
}

void Gstate::setExit(GstateExit* exit)
{
	if(m_exit != NULL)
		delete m_exit;

	m_exit = exit;
}

/*
void Gstate::setStateVars(EList<Gstmt*>* vars)
{
	//if(m_exit != NULL)
	//	delete m_exit;

	//m_stateVarList = vars;	
	setStmts(vars);
}
*/
void Gstate::addStateVar(GlocalVar* lvar)
{
	//if(m_stateVarList == NULL) 
	//	m_stateVarList = new EList<Gstmt*>();
	//m_stateVarList->addTail(lvar);

	addStmt(lvar);
}

/*
void Gstate::addSynchID(int id)
{
	m_synchList.push_back(id);
}

void Gstate::copySynchIDs(std::vector<int>& a)
{
	std::vector<int>::iterator it;
	std::vector<int>::iterator pos;
	for(it=m_synchList.begin(); it<m_synchList.end(); it++){
		pos = std::find(a.begin(), a.end(), (*it));
		if(pos == a.end()){//중복되지 않으면
			a.push_back(*it);
		}
	}
}
*/
void Gstate::setActionBlock(GstmtBlock* block)
{
	if(block != NULL){	
		if(dynamic_cast<GstateDo*>(block) ){
			setDo((GstateDo*)block);
		}
		if(dynamic_cast<GstateEntry*>(block)){
			setEntry((GstateEntry*)block);
		}
		if(dynamic_cast<GstateExit*>(block) ){
			setExit((GstateExit*)block);
		}		
	}
}

Gtoken* Gstate::getName()
{
	return m_name;
}

int Gstate::getModifier()
{
	return m_mod;
}

EList<Gstmt*>* Gstate::getStateVars()
{
	//return m_stateVarList;
	return getStmts();
}

GstateTrans* Gstate::getTransition()
{
	return m_transition;
}

GstateDo* Gstate::getDoBlock()
{
	if(m_do != NULL)
		return m_do;
	else
		return 0;
}

GstateEntry* Gstate::getEntryBlock()
{
	if(m_entry != 0)
		return m_entry;
	else
		return 0;
}

GstateExit*	Gstate::getExitBlock()
{
	if(m_exit != NULL)
		return m_exit;
	else
		return 0;
}

void Gstate::print(unsigned int indent)
{	
	titlePrint(indent, std::string("STATE"));

	if(m_name != NULL)
		m_name->print(indent+1);

	if(m_transition != NULL)
		m_transition->print(indent+1);

	titlePrint(indent+1, std::string("ENTRY"));
	if(m_entry != NULL)
		m_entry->print(indent+2);

	titlePrint(indent+1, std::string("DO"));
	if(m_do != NULL)
		m_do->print(indent+2);

	titlePrint(indent+1, std::string("EXIT"));
	if(m_exit != NULL)
		m_exit->print(indent+2);

}