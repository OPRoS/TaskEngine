/*
 * Gcontext.cpp
 *
 *  Created on: 2009. 3. 3
 *      Author: jinseo
 */

#include "stxmodel/Gcontext.h"

Gcontext::Gcontext()
{
	m_name = NULL;
	m_ruleList = NULL;
	m_ctxList = NULL;
}

Gcontext:: ~Gcontext()
{
	if(m_name != NULL) delete m_name;

	if(m_ruleList != NULL){
		while(m_ruleList->getHead() !=NULL && m_ruleList->moveHead()){
			Grule* obj = m_ruleList->getCurObject();
			delete obj;
			m_ruleList->delHead();
		}
		delete m_ruleList;
	}

	if(m_ctxList != NULL){
		while(m_ctxList->getHead() !=NULL && m_ctxList->moveHead()){
			Gcontext* obj = m_ctxList->getCurObject();
			delete obj;
			m_ctxList->delHead();
		}
		delete m_ctxList;
	}
}

void Gcontext::setName(Gtoken* name)
{
	m_name = name;
}

void Gcontext::addRule(Grule* rule)
{
	if(m_ruleList == NULL)
		m_ruleList = new EList<Grule*>();

	m_ruleList->addTail(rule);
}

void Gcontext::addContext(Gcontext* ctx)
{
	if(m_ctxList == NULL)
		m_ctxList = new EList<Gcontext*>();

	m_ctxList->addTail(ctx);
}
