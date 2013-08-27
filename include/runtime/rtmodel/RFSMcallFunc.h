/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "runtime/rtmodel/RFSMcall.h"
#include "runtime/rtmodel/RFSMvalue.h"
#include "runtime/rtmodel/RFSMboolable.h"
#include "ds/EList.h"

class RFSMcallFunc : public RFSMcall, public RFSMvalue, public RFSMboolable
{
private:
	unsigned int	m_rtnType;
	bool			m_isAction;

public:
	RFSMcallFunc(std::string, std::string, std::string name, bool, std::string file, int line);
	virtual ~RFSMcallFunc(void);

	unsigned int getType();   //원래는 getRtnType인데 RFSMvalue에 getType가상함수가 있기 때문에...
	void setRtnType(unsigned int);

	std::string toString();
	bool isAction();
};
