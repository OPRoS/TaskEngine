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
#include "runtime/rtmodel/RFSMboolable.h"
#include "common/Gdatatype.h"
#include <string>

class RFSMcondValue : public RFSMvalue
{
private:
	RFSMboolable*	m_condSymbol;		//bool타입의 symbol (model)
	RFSMvalue*		m_condT_Rvalue;		//symbol이 참일 때 값
	RFSMvalue*		m_condF_Rvalue;		//symbol이 거짓일 때 값

public:
	RFSMcondValue(RFSMboolable*, RFSMvalue*, RFSMvalue*, std::string file, int line);
	virtual ~RFSMcondValue(void);

	RFSMvalue* getTrueValue();
	RFSMvalue* getFalseValue();
	RFSMboolable* getCondition();

	unsigned int getType();
	std::string toString();
};
