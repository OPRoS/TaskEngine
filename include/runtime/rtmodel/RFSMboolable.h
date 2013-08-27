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
#include "ds/EList.h"
#include "runtime/rtmodel/RFSMObject.h"

class RFSMboolable : public RFSMObject
{
private:
	bool m_hasNot;

public:
	RFSMboolable(std::string m_className);
	virtual ~RFSMboolable(void);

	void setHasNot();
	bool hasNot();

	std::string toString();
	static std::string toString(RFSMboolable*);

};
