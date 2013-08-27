/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/Gbehavior.h"

Gbehavior::Gbehavior(std::string file, int eol, int blockid, std::string blockname, std::string blockpath): GcstBlock(file, eol, blockid, blockname, blockpath)
{	
	m_parameters	= NULL;
	m_states		= NULL;	
}

Gbehavior:: ~Gbehavior(void)
{
	if(m_parameters != NULL){
		while(m_parameters->moveHead()){
			Gparameter* obj = m_parameters->getCurObject();
			delete obj;
			m_parameters->delHead();
		}
		delete m_parameters;
	}

	if(m_states != NULL){
		while(m_states->moveHead()){
			Gstate* obj = m_states->getCurObject();
			delete obj;
			m_states->delHead();
		}
		delete m_states;
	}

	m_parameters = NULL;
	m_states = NULL;
}

void Gbehavior::setParameters(EList<Gparameter*>* params)
{
	m_parameters = params;
}

void Gbehavior::addParameter(Gparameter* param)
{
	if(m_parameters == NULL)
		m_parameters = new EList<Gparameter*>();

	m_parameters->addTail(param);
}

void Gbehavior::setStates(EList<Gstate*>* states)
{
	if(m_states != NULL)
		delete m_states;
	m_states = states;
}

EList<Gstate*>* Gbehavior::getStates()
{
	return m_states;
}
/*
std::vector<int> Gbehavior::getSynchInfo()
{
	std::vector<int> synchlist;
	if(m_states != 0 && m_states->getSize()>0 && m_states->moveHead()){
		do{
			Gstate *state = m_states->getCurObject();
			state->copySynchIDs(synchlist);
		}
		while(m_states->moveNext());
	}
	return synchlist;
	
}
*/
EList<Gparameter*>* Gbehavior::getParameters()
{
	return m_parameters;
}

void Gbehavior::print(unsigned int indent)
{
	titlePrint(indent, std::string("TASK"));

	if(getName() != NULL) getName()->print(indent+1);

	if(m_parameters !=NULL && m_parameters->moveHead()){
		do{
			Gparameter *param = m_parameters->getCurObject();
			param->print(indent+1);
		}
		while(m_parameters->moveNext());
	}

	if(m_states !=NULL && m_states->moveHead()){
		do{
			Gstate *state = m_states->getCurObject();
			state->print(indent+1);
		}
		while(m_states->moveNext());
	}
}
