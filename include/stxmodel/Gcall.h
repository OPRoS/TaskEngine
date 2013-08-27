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
#include "GstmtExpr.h"
#include "Gstmt.h"
#include "Gtoken.h"

class Gcall: public Ggram
{
private:
	Gtoken*				m_name;
	EList<GstmtExpr*>*	m_params;

public:
	Gcall(std::string, int, std::string, std::string);
	virtual ~Gcall(void);

	char* toString(){return NULL;};
	void print(unsigned int);

	void setName(Gtoken*);
	void setParams(EList<GstmtExpr*>*);

	Gtoken* getName();
	EList<GstmtExpr*>* getParameters();
};
