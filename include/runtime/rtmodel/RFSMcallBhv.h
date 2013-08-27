/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "runtime/rtmodel/RFSMcall.h"
#include <string>

class RFSMcallBhv : public RFSMcall
{
private:
	std::string m_nextTarget;
	int			m_period;

public:
	RFSMcallBhv(std::string name, std::string, std::string file, std::string blockpath, std::string blockname, int, int line);
	virtual ~RFSMcallBhv(void);

	std::string toString();
	std::string getNextTarget();
	int			getPeriod();
};
