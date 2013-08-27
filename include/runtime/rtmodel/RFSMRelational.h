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
#include "runtime/rtmodel/RFSMboolable.h"
#include "runtime/rtmodel/RFSMbinaryOp.h"
#include "common/Gdatatype.h"

class RFSMRelational: public RFSMboolable, public RFSMbinaryOp
{

public:
	RFSMRelational(unsigned int, RFSMvalue* lval, RFSMvalue* rval);
	virtual ~RFSMRelational(void);

	//unsigned int getType(){return TYP_BOOL;};

	std::string toString();
};
