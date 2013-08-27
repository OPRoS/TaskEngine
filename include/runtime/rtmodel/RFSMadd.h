/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "runtime/rtmodel/RFSMvalue.h"
#include "common/Gdatatype.h"
#include "runtime/rtmodel/RFSMbinaryOp.h"


class RFSMadd : public RFSMbinaryOp, public RFSMvalue
{
public:
	RFSMadd(unsigned int op, RFSMvalue* left, RFSMvalue* right, std::string file, int line);
	virtual ~RFSMadd(void);

	//bool isTypeComputable(); //int float가능, string이나 bool이 있으면 안된다.
	unsigned int getType();//int float는 float를 리턴

	std::string toString();
};
