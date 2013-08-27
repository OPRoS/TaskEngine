/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "runtime/rtmodel/RFSMnamedItem.h"
#include "runtime/rtmodel/RFSMvalue.h"
#include <string>

class RFSMsym : public RFSMnamedItem, public RFSMvalue
{
public:
	RFSMsym(std::string, std::string, std::string, std::string filename, int);
	virtual ~RFSMsym(void);
};
