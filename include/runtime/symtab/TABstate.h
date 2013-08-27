/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "runtime/rtmodel/RFSMnamedItem.h"

class TABstate : public RFSMnamedItem
{
private:
	std::string m_taskName;

public:
	TABstate(std::string, std::string);
	virtual ~TABstate(void);

	std::string getTaskName();
};
