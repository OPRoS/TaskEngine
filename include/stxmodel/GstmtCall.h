/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include <iostream>
#include "Gcall.h"
#include "GstmtExpr.h"
#include "Gstmt.h"
#include "Gtoken.h"

class GstmtCall : public Gstmt
{
private:
	Gcall*	m_call;

public:
	GstmtCall(std::string, int, std::string, std::string);
	virtual ~GstmtCall(void);

	char* toString(){return NULL;};
	void print(unsigned int);

	void setCall(Gcall*);
	Gcall* getCall();

	//void addParamName(std::string*);
	//void addValue(std::string*);
	//void setParamsName(EList<std::string*>*);
	//void setValues(EList<std::string*>*);
};
