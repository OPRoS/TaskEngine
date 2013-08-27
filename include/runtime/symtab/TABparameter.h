/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include <string>

class TABparameter
{
private:
	std::string		m_type;
	std::string		m_name;

public:
	TABparameter(std::string type, std::string name);
	virtual ~TABparameter(void);

	std::string		getType();
	std::string		getName();
};