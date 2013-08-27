/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Scenario 
** Date:        2011.08.15
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "scenario/SBhvSeq.h"
#include <vector>

class SC
{
private:
	std::string				m_scname;
	std::vector<SBhvSeq*>	bslist;	
	std::string				m_type;

public:
	SC(void);
	virtual ~SC(void);

	std::string				getSCname	(void);
	std::string				getType		(void);
	std::vector<SBhvSeq*>	getBSlist	(void);
	void					addBS		(SBhvSeq*);
	void					setName		(std::string);
	void					setType		(std::string);
};
