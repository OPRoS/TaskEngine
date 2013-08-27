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

class TABaction : public TABcall
{
public:
	TABaction(std::string, EList<TABparameter*>*);
	virtual ~TABaction(void);
};
