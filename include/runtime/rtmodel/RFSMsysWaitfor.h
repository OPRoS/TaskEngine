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


class RFSMsysWaitfor :  public RFSMcall
{
private:
	RFSMvalue*	m_Value;
	int			m_startTime;
	int			m_timeout;

public:
	RFSMsysWaitfor(std::string, std::string file, std::string blockpath, std::string blockname, int line);
	virtual ~RFSMsysWaitfor(void);

	void setValue(RFSMvalue*);
	RFSMvalue* getValue();

	std::string toString();
};