/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once
#include "TABparameter.h"
#include "TABcall.h"

class TABbehavior : public TABcall
{

public:
	TABbehavior(std::string, EList<TABparameter*>*);
	virtual ~TABbehavior(void);
};
