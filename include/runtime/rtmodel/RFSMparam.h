/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include <string>
#include "runtime/rtmodel/RFSMvalue.h"

class RFSMparam
{
private:
	std::string m_name;
	RFSMvalue*	m_value;

public:
	RFSMparam(std::string, RFSMvalue*);
	virtual ~RFSMparam(void);

	std::string getName();
	RFSMvalue* getValue();
};
