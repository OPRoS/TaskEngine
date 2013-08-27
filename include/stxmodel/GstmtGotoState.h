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

class GstmtGotoState : public Gstmt
{
private:
	//GstmtCall*	m_behavior;   //goto behavior, connector
	Gtoken*		m_stateName;  //state
	//Gtoken*		m_nextTarget; //next target

public:
	GstmtGotoState(std::string file, int eol, std::string blockname, std::string blockpath);
	virtual ~GstmtGotoState(void);

	char* toString(){return NULL;};
	void print(unsigned int);

	void setStateName(Gtoken*);
	//void setBehavior(GstmtCall*);
	//void setNextTarget(Gtoken*);

	Gtoken*		getStateName();	
	//GstmtCall*	getBehavior();
	//Gtoken*		getNextTarget();
};
