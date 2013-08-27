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
#include <vector>
#include "ds/Estring.h"
#include "ds/EList.h"

class RFSMnamedItem
{
protected:
	std::string m_name;

public:
	RFSMnamedItem(std::string);
	RFSMnamedItem(char*);
	virtual ~RFSMnamedItem(void);

	void appendName(std::string);
	bool isSameName(std::string);	
	void setName(std::string);

	std::string getName();
	static std::string removePath(std::string);

};
