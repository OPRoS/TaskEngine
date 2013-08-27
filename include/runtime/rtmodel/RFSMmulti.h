/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "runtime/rtmodel/RFSMbinaryOp.h"
#include "runtime/rtmodel/RFSMvalue.h"
#include "common/Gdatatype.h"

class RFSMmulti : public RFSMbinaryOp, public RFSMvalue
{
public:
	RFSMmulti(unsigned int op, RFSMvalue* left, RFSMvalue* right, std::string file, int line);
	virtual ~RFSMmulti(void);

	unsigned int getType();//int float는 float를 리턴

	std::string toString();
};
