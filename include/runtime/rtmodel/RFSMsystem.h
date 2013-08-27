/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "runtime/rtmodel/RFSMsym.h"

class RFSMsystem : public RFSMsym
{

public:
	RFSMsystem(std::string, std::string, unsigned int, std::string name, std::string file, int line);
	virtual ~RFSMsystem(void);

	unsigned int getType();
	std::string toString();
};
