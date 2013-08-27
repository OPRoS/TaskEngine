/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/GstmtGotoState.h"

GstmtGotoState::GstmtGotoState(std::string file, int eol, std::string blockname, std::string blockpath):Gstmt(STMT_goto, file, eol, blockname, blockpath)
{
	m_stateName		= NULL;
	//m_behavior		= NULL;
	//m_nextTarget	= NULL;
}

GstmtGotoState:: ~GstmtGotoState(void)
{
	if(m_stateName != NULL)
		delete m_stateName;
	//if(m_behavior != NULL)
	//	delete m_behavior;
	//if(m_nextTarget != NULL)
	//	delete m_nextTarget;
}

void GstmtGotoState::setStateName(Gtoken* stateName)
{
	m_stateName = stateName;
}

Gtoken* GstmtGotoState::getStateName()
{
	return m_stateName;
}
/*
void GstmtGotoState::setBehavior(GstmtCall* behavior, std::string nextTarget)
{
	m_behavior = behavior;
	m_nextTarget = nextTarget;
}

void GstmtGotoState::setNextTarget(std::string nextTarget)
{
	m_nextTarget = nextTarget;
}

GstmtCall* GstmtGotoState::getBehavior()
{
	return m_behavior;
}

Gtoken* GstmtGotoState::getNextTarget()
{
	return m_nextTarget;
}
*/
void GstmtGotoState::print(unsigned int indent)
{
	titlePrint(indent, std::string("GOTO"));
	if(m_stateName != NULL) m_stateName->print(indent+1);
}
