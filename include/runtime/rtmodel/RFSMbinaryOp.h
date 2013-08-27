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
#include "common/Gdatatype.h"


class RFSMbinaryOp
{
protected:
	unsigned int m_op;
	RFSMvalue* m_lval;
	RFSMvalue* m_rval;

public:
	RFSMbinaryOp(unsigned int, RFSMvalue* lval, RFSMvalue* rval);
	virtual ~RFSMbinaryOp(void);

	unsigned int getOperator();
	RFSMvalue* getLeftOperand();
	RFSMvalue* getRightOperand();

	std::string toString();
};
