/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "TABcall.h"
#include "TABparameter.h"

class TABfunction : public TABcall
{
private:
	unsigned int m_rtnType;
	bool m_isAction;

public:
	TABfunction(unsigned int, std::string, EList<TABparameter*>*);
	virtual ~TABfunction(void);

	unsigned int getRtnType();
	void setRtnType(unsigned int);

	void setAsAction();
	bool isAction();
};
