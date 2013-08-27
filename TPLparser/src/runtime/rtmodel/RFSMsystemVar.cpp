/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMsystemVar.h"

RFSMsystemVar::RFSMsystemVar(void)
{
	//m_sysVars.addHead("STATE_TIME");
	//m_sysVars.addHead("TASK_TIME");
}

RFSMsystemVar:: ~RFSMsystemVar(void)
{
}

void RFSMsystemVar::setStartTime(std::string name)
{
	time_t seconds;
	seconds = time (NULL)/3600;
	float t;

	typedef std::pair <std::string, float> valpair;
	m_startTimeTAB.insert(valpair(name, t));
}

//stateTime
float RFSMsystemVar::getStateTime(std::string stateName)
{
	float val = m_startTimeTAB.find(stateName)->second;
	return val;
}

//taskTime
float RFSMsystemVar::getTaskTime(std::string taskName)
{
	float val = m_startTimeTAB.find(taskName)->second;
	return val;
}

/*
bool RFSMsystemVar::isSysVar(std::string name)
{
	if(m_sysVars.moveHead() && m_sysVars.getSize()>0){
		std::string str = m_sysVars.getCurObject();
		if(str.compare(name)==0)
			return true;
	}

	return false;
}
*/
