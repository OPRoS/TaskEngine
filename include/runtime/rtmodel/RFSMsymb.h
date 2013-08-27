/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "runtime/rtmodel/RFSMstmt.h"

class RFSMsymb : public RFSMstmt
{
private:
	std::string m_symName;

public:
	RFSMsymb(std::string, std::string file, std::string blockpath, std::string blockname, int line);
	virtual ~RFSMsymb(void);
	
	std::string toString();

};