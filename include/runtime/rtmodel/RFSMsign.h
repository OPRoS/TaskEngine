/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "runtime/rtmodel/RFSMvalue.h"

class RFSMsign : public RFSMvalue
{
private:
	bool			m_hasMinus;
	RFSMvalue*		m_value;

public:
	RFSMsign(bool hasMinus, RFSMvalue* value, std::string file, int line);
	virtual ~RFSMsign(void);

	bool hasMinus();
	void setMinus();
	RFSMvalue* getValue();

	unsigned int getType();
};
