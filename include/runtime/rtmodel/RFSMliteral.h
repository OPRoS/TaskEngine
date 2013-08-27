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
#include "runtime/rtmodel/RFSMvalue.h"
#include "runtime/rtmodel/RFSMboolable.h"

class RFSMliteral : public RFSMvalue, public RFSMboolable
{
private:
	std::string		m_data;
	//unsigned int	m_type; //RFSMvalue에 m_type이 존재함

public:
	RFSMliteral(std::string, std::string, std::string, unsigned int, std::string, int);
	virtual ~RFSMliteral(void);

	unsigned int getType();
	std::string getData();

	std::string toString();

};
