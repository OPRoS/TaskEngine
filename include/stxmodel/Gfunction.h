/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "Gaction.h"

class Gfunction : public Gaction
{
private:
	Gtoken*	m_rtnType;
	bool m_isAction;

public:
	Gfunction(std::string, int eol, std::string, std::string);
	virtual ~Gfunction(void);

	void setReturnType(Gtoken* rtnType);
	Gtoken* getReturnType();

	void setAsAction();
	bool isAction();
};
