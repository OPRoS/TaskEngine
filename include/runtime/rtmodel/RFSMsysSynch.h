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
#include "runtime/rtmodel/RFSMliteral.h"
#include "util/Utility.h"


class RFSMsysSynch:  public RFSMcall
{
private:
	RFSMvalue*	m_Value;

public:
	RFSMsysSynch(std::string, std::string file, std::string blockpath, std::string blockname, int line);
	virtual ~RFSMsysSynch(void);

	void setValue(RFSMvalue*);
	RFSMvalue* getValue();
	int getSynchID();

	std::string toString();
};