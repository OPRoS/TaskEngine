/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "run/control/ExeTransition.h"
//static int cn = 0;
//static int dn = 0;
ExeTransition::ExeTransition(int transType, std::string name, std::string nextTarget, int stmtline, int period) : RFSMnamedItem(name)
{	
	m_transType		= transType;
	m_params		= NULL;
	m_nextTarget	= nextTarget;
	m_stmtLine		= stmtline;
	m_period		= period;
	//m_exeBhv	= NULL;
//printf("ccccccccccccccccccccc <%s> %d\n", m_name.c_str(), ++cn);
}

ExeTransition:: ~ExeTransition(void)
{
//printf("ddddddddddddddddddddd <%s> %d\n", m_name.c_str(), ++dn);
	//ExeTransition�� TaskRunner.enterState()���� moveto������ ���� �Ҵ�Ǵµ�
	//enterState()�Լ����� ����� ������ ���⼭ ������ �ʴ´�.
	if(m_params != NULL){ 
		m_params->clear();
		delete m_params;
		m_params = NULL;
	}
}

void ExeTransition::setTransType(int type)
{
	m_transType = type;
}


/*
void ExeTransition::setRunType(int type)
{
	m_runType = type
}

void ExeTransition::setJoinType(int type)
{
	m_joinType = type;
}
*/
void ExeTransition::setParameters(ValueMap* params)
{
	m_params = params;
}

int ExeTransition::getTransType()
{
	return m_transType;
}

/*
int ExeTransition::getRunType()
{
	return m_runType;
}
int ExeTransition::getJoinType()
{
	return m_joinType;
}
*/
ValueMap* ExeTransition::getParameters()
{
	return m_params;
}


std::string ExeTransition::getNextTargetState()
{
	return m_nextTarget;
}

int ExeTransition::getStmtLine()
{
	return m_stmtLine;
}

int ExeTransition::getPeriod()
{
	return m_period;
}