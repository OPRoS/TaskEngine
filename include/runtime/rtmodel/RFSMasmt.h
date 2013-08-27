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
#include "runtime/rtmodel/RFSMvariable.h"
#include "common/Gdatatype.h"
#include "runtime/rtmodel/RFSMstmt.h"
#include <string>

class RFSMasmt : public RFSMvalue, public RFSMstmt
{
private:
	unsigned int		m_op;
	RFSMvariable*		m_lValue; //¿ÞÂÊ ½É¹ú(model.var)
	RFSMvalue*			m_rValue;

public:
	RFSMasmt(unsigned int op, RFSMvariable* lvar, RFSMvalue* rval, std::string file, int line);
	virtual ~RFSMasmt(void);

	unsigned int getType();
	RFSMvariable* getLvalue();
	RFSMvalue*  getRvalue();
	unsigned int isAssignable();

	std::string toString();

};
