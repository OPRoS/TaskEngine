/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Scenario 
** Date:        2011.08.15
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include <string>

class STask
{
private:
	std::string	m_name;
	std::string	m_start;

public:
	STask(void);
	virtual ~STask(void);

	std::string	getName	(void);
	std::string	getStart(void);

	void setName	(std::string);
	void setStart	(std::string);
};
