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
#include "runtime/rtmodel/RFSMnamedItem.h"

class TABenum : public RFSMnamedItem
{
private:
	std::string		m_typeName;
	unsigned int	m_value;
	unsigned int	m_type;

public:
	TABenum(std::string typeName, std::string enumName, unsigned int enumValue);
	virtual ~TABenum(void);
	
	void setType(unsigned int);

	unsigned int getType();
	unsigned int getValue();
	std::string	 getTypeName();	

};
