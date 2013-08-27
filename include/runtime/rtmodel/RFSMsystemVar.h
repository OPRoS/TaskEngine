/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "ds/EList.h"
#include <string>
#include <map>
#include <time.h>


class RFSMsystemVar
{
private:
	//EList<std::string>				m_sysVars;
	std::map<std::string, float>	m_startTimeTAB;


public:
	RFSMsystemVar(void);
	virtual ~RFSMsystemVar(void);

	//static bool isSysVar(std::string name);

	void setStartTime(std::string name);

	//stateTime	
	float getStateTime(std::string stateName);

	//taskTime
	float getTaskTime(std::string taskName);
};
