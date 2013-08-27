/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMstmt.h"

RFSMstmt::RFSMstmt(std::string className, std::string file, std::string blockpath, std::string blockname, int line)
	:/*RFSMBreakPoint(file), */
	 RFSMBlockInfo(blockpath, blockname, file, line), 
	 RFSMObject(className)
{
	m_isPerformed = false;
}

RFSMstmt:: ~RFSMstmt(void)
{
}

void RFSMstmt::setPerformed()
{
	m_isPerformed = true;
}

void RFSMstmt::setNotPerformed()
{
	m_isPerformed = false;
}

bool RFSMstmt::isPerformed()
{
	return m_isPerformed;
}