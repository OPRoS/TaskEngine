/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/Genum.h"

Genum::Genum(void)
{
	m_name = NULL;
	m_elist = NULL;
}

Genum:: ~Genum(void)
{
	//if(m_elist !=NULL)delete m_elist;
	if(m_name != NULL)delete m_name;

	if(m_elist != NULL){
		while(m_elist->getHead() !=NULL && m_elist->moveHead()){
			Gtoken* obj = m_elist->getCurObject();
			delete obj;
			m_elist->delHead();
		}
		delete m_elist;
	}
}

void Genum::setName(Gtoken* name)
{
	m_name = name;
}

void Genum::addElement(Gtoken* element)
{
	if(m_elist == NULL)
		m_elist = new EList<Gtoken*>();

	m_elist->addTail(element);
}

Gtoken* Genum::getName()
{
	return m_name;
}

EList<Gtoken*>* Genum::getElements()
{
	return m_elist;
}
