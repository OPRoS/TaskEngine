/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "runtime/rtmodel/RFSMtransition.h"
#include "runtime/rtmodel/RFSMifStmt.h"

class RFSMtransition
{
private:
	std::vector<RFSMifStmt*>* m_ifstmts;

public:
	RFSMtransition(void);
	virtual ~RFSMtransition(void);

	void setIfstmts(std::vector<RFSMifStmt*>*);
	std::vector<RFSMifStmt*>* getIfstmts();
};
