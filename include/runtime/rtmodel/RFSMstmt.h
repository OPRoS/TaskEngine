/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "runtime/rtmodel/RFSMObject.h"
#include "runtime/rtmodel/RFSMBreakPoint.h"
#include "runtime/rtmodel/RFSMBlockInfo.h"
#include <string>

class RFSMstmt : /*public RFSMBreakPoint,*/ public RFSMBlockInfo, public RFSMObject
{
private:
	bool m_isPerformed; //waitfor 때문에 정의했는데 필요없으면 지우자..

public:
	RFSMstmt(std::string, std::string file, std::string blockpath, std::string blockname, int line);
	virtual ~RFSMstmt(void);

	void setPerformed	(void);
	void setNotPerformed(void);
	bool isPerformed	(void);
};

