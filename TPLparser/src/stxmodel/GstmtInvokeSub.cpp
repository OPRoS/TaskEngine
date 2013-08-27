/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/GstmtInvokeSub.h"

GstmtInvokeSub::GstmtInvokeSub(std::string file, int eol, std::string blockname, std::string blockpath):Gstmt(STMT_CallBhv, file, eol, blockname, blockpath)
{
	m_behavior		= NULL;
	m_nextTarget	= NULL;
	m_period		= -1;
}

GstmtInvokeSub:: ~GstmtInvokeSub(void)
{
	if(m_behavior != NULL)
		delete m_behavior;
	if(m_nextTarget != NULL)
		delete m_nextTarget;
}

void GstmtInvokeSub::setInvokeCall(Gcall* behavior)
{
	m_behavior = behavior;	
}

void GstmtInvokeSub::setNextTarget(Gtoken* nextTarget)
{
	m_nextTarget = nextTarget;
}

void GstmtInvokeSub::setBoolVar(Gtoken* var)
{
	m_boolVar = var;
}

Gcall* GstmtInvokeSub::getInvokeCall()
{
	return m_behavior;
}

Gtoken* GstmtInvokeSub::getNextTarget()
{
	return m_nextTarget;
}

Gtoken* GstmtInvokeSub::getBoolVar()
{
	return m_boolVar;
}

void GstmtInvokeSub::setPeriod(int period)
{
	m_period = period;
}

int GstmtInvokeSub::getPeriod()
{
	return m_period;
}


void GstmtInvokeSub::print(unsigned int indent)
{
	titlePrint(indent, std::string("INVOKE"));
}
