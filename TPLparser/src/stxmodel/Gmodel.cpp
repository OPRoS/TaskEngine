/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/Gmodel.h"

Gmodel::Gmodel(void)
{
	m_name = NULL;
	m_varList = NULL;
	m_modelList =NULL;
	m_funcList = NULL;
}

Gmodel:: ~Gmodel(void)
{
	if(m_name != NULL) delete m_name;
	//if(m_varList != NULL) delete m_varList;
	//if(m_modelList != NULL) delete m_modelList;	

	
	if(m_varList != NULL){
		while(m_varList->getHead() !=NULL && m_varList->moveHead()){
			GmodelVar* obj = m_varList->getCurObject();
			delete obj;
			m_varList->delHead();
		}
		delete m_varList;
	}

	if(m_modelList != NULL){
		while(m_modelList->getHead() !=NULL && m_modelList->moveHead()){
			Gmodel* obj = m_modelList->getCurObject();
			delete obj;
			m_modelList->delHead();
		}
		delete m_modelList;
	}

	if(m_funcList != NULL){
		while(m_funcList->getHead() !=NULL && m_funcList->moveHead()){
			Gfunction* obj = m_funcList->getCurObject();
			delete obj;
			m_funcList->delHead();
		}
		delete m_funcList;
	}
}

void Gmodel::addVariable(GmodelVar* var)
{
	if(m_varList == NULL)
		m_varList = new EList<GmodelVar*>();

	m_varList->addTail(var);
}

void Gmodel::addModel(Gmodel* model)
{
	if(m_modelList == NULL)
		m_modelList = new EList<Gmodel*>();

	m_modelList->addTail(model);
}

void Gmodel::addFunc(Gfunction* func)
{
	if(m_funcList == NULL)
		m_funcList = new EList<Gfunction*>();

	m_funcList->addTail(func);
}

void Gmodel::setName(Gtoken* name)
{
	m_name = name;
}

Gtoken* Gmodel::getName()
{
	return m_name;
}

EList<GmodelVar*>* Gmodel::getVariables()
{
	return m_varList;
}
	
EList<Gmodel*>* Gmodel::getModels()
{
	return m_modelList;
}

EList<Gfunction*>* Gmodel::getFunctions()
{
	return m_funcList;
}