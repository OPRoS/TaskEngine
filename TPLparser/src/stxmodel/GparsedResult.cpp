/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/GparsedResult.h"

GparsedResult::GparsedResult(void)
{
	m_enumlist		= NULL;
	m_modellist		= NULL;
	m_ctxlist		= NULL;
	m_actionlist	= NULL;
	m_bhvlist		= NULL;
	m_conlist		= NULL;
	m_tasklist		= NULL;
}

GparsedResult:: ~GparsedResult(void)
{

	//if(m_enumlist != NULL)	delete m_enumlist;
	//if(m_modellist != NULL)	delete m_modellist;
	//if(m_actionlist != NULL)	delete m_actionlist;
	//if(m_bhvlist != NULL)		delete m_bhvlist;

	if(m_enumlist != NULL){
		while(m_enumlist->moveHead()){
			Genum* obj = m_enumlist->getCurObject();
			delete obj;
			m_enumlist->delHead();
		}
		delete m_enumlist;
	}
	if(m_modellist != NULL){
		while(m_modellist->moveHead()){
			Gmodel* obj = m_modellist->getCurObject();
			delete obj;
			m_modellist->delHead();
		}
		delete m_modellist;
	}
	if(m_ctxlist != NULL){
		while(m_ctxlist->moveHead()){
			Gcontext* obj = m_ctxlist->getCurObject();
			delete obj;
			m_ctxlist->delHead();
		}
		delete m_ctxlist;
	}
	if(m_actionlist != NULL){
		while(m_actionlist->moveHead()){
			Gaction* obj = m_actionlist->getCurObject();
			delete obj;
			m_actionlist->delHead();
		}
		delete m_actionlist;
	}
	if(m_bhvlist != NULL){
		while(m_bhvlist->moveHead()){
			Gbehavior* obj = m_bhvlist->getCurObject();
			delete obj;
			m_bhvlist->delHead();
		}
		delete m_bhvlist;
	}
	if(m_conlist != NULL){
		while(m_conlist->moveHead()){
			Gconnector* obj = m_conlist->getCurObject();
			delete obj;
			m_conlist->delHead();
		}
		delete m_conlist;
	}	
	if(m_tasklist != NULL){
		while(m_tasklist->moveHead()){
			Gtask* obj = m_tasklist->getCurObject();
			delete obj;
			m_tasklist->delHead();
		}
		delete m_tasklist;
	}
	if(m_sevents.size() > 0){
		for(unsigned int i = 0; i < m_sevents.size(); i++){
			delete m_sevents[i];
		}
		m_sevents.clear();
	}
	if(m_sbhvs.size() > 0){
		for(unsigned int i = 0; i < m_sbhvs.size(); i++){
			delete m_sbhvs[i];
		}
		m_sbhvs.clear();
	}
	if(m_sclist.size() > 0){
		for(unsigned int i = 0; i < m_sclist.size(); i++){
			delete m_sclist[i];
		}
		m_sclist.clear();
	}
	if(m_stasks.size() > 0){
		for(unsigned int i = 0; i < m_stasks.size(); i++){
			delete m_stasks[i];
		}
		m_stasks.clear();
	}
}
/*
void GparsedResult::setInclude(Ginclude* incl)
{
	if(m_incl != NULL)delete incl;
	m_incl = incl;
}
*/
void GparsedResult::addEnum(Genum *enu)
{
	if(m_enumlist == NULL)
		m_enumlist = new EList<Genum*>();
	m_enumlist->addTail(enu);
}

void GparsedResult::addModel(Gmodel *model)
{
	if(m_modellist == NULL)
		m_modellist = new EList<Gmodel*>();
	m_modellist->addTail(model);
}

void GparsedResult::addContext(Gcontext *ctx)
{
	if(m_ctxlist == NULL)
		m_ctxlist = new EList<Gcontext*>();
	m_ctxlist->addTail(ctx);
}

void GparsedResult::addAction(Gaction *action)
{
	if(m_actionlist == NULL)
		m_actionlist = new EList<Gaction*>();
	m_actionlist->addTail(action);
}


void GparsedResult::addTask(Gtask* task)
{
	if(m_tasklist == NULL)
		m_tasklist = new EList<Gtask*>();

	m_tasklist->addTail(task);
}

void GparsedResult::addbehavior(Gbehavior* task)
{
	if(m_bhvlist == NULL)
		m_bhvlist = new EList<Gbehavior*>();

	m_bhvlist->addTail(task);
}

void GparsedResult::addconnector(Gconnector* con)
{
	if(m_conlist == NULL)
		m_conlist = new EList<Gconnector*>();

	m_conlist->addTail(con);
}
void GparsedResult::addSEvent(SEvent* se)
{
	m_sevents.push_back(se);
}

void GparsedResult::addSBehavior(SBehavior* sbhv)
{
	m_sbhvs.push_back(sbhv);
}

void GparsedResult::addSC(SC* sc)
{
	m_sclist.push_back(sc);
}

void GparsedResult::addSTask(STask* stask)
{
	m_stasks.push_back(stask);
}

EList<Genum*>*	GparsedResult::getEnums()
{
	return m_enumlist;
}

EList<Gmodel*>* GparsedResult::getModels()
{
	return m_modellist;
}

EList<Gcontext*>* GparsedResult::getContexts()
{
	return m_ctxlist;
}

EList<Gaction*>* GparsedResult::getActions()
{
	return m_actionlist;
}

EList<Gtask*>* GparsedResult::getTasks()
{
	return m_tasklist;
}

EList<Gbehavior*>* GparsedResult::getbehaviors()
{
	return m_bhvlist;
}

EList<Gconnector*>* GparsedResult::getconnectors()
{
	return m_conlist;
}

std::vector<SEvent*> GparsedResult::getSEvents()
{
	return m_sevents;
}

std::vector<SBehavior*>	GparsedResult::getSBehaviors()
{
	return m_sbhvs;
}

std::vector<SC*> GparsedResult::getSCs()
{
	return m_sclist;
}

std::vector<STask*> GparsedResult::getSTasks()
{
	return m_stasks;
}