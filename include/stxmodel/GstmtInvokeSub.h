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
#include "stxmodel/Gstmt.h"
#include "stxmodel/Gtoken.h"
#include "stxmodel/GstmtCall.h"

class GstmtInvokeSub : public Gstmt
{
private:
	Gcall*		m_behavior;   //goto behavior, connector
	Gtoken*		m_nextTarget; //next target
	Gtoken*		m_boolVar;
	int			m_period;	//run, with¿« º”º∫


public:
	GstmtInvokeSub(std::string file, int eol, std::string blockname, std::string blockpath);
	virtual ~GstmtInvokeSub(void);

	char* toString(){return NULL;};
	void print(unsigned int);
	
	void setInvokeCall(Gcall*);
	void setNextTarget(Gtoken*);
	void setBoolVar(Gtoken*);
	
	Gcall*	getInvokeCall();
	Gtoken*	getNextTarget();
	Gtoken*	getBoolVar();

	void setPeriod(int period);
	int getPeriod();


};
