/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "runtime/rtmodel/RFSMlogicOp.h"
#include "runtime/rtmodel/RFSMboolable.h"
#include <string>

class RFSMand : public RFSMlogicOp, public RFSMboolable
{
public:
	RFSMand(void);
	virtual ~RFSMand(void);

	std::string toString();
};
