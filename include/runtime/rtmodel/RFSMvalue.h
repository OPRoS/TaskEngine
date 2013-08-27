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
#include "runtime/rtmodel/RFSMBlockInfo.h"
#ifndef _WIN32
#include <typeinfo>
#endif

class RFSMvalue : public RFSMBlockInfo
{
protected:	
	unsigned int	m_type;			//int, float, string	

public:
	RFSMvalue(std::string, std::string, std::string file, int line);
	virtual ~RFSMvalue(void);

	void setType(unsigned int);	

	virtual unsigned int getType() = 0;	
	std::string toString();	

	static std::string toString(RFSMvalue* value);
};
