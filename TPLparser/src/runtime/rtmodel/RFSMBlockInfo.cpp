/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMBlockInfo.h"

RFSMBlockInfo::RFSMBlockInfo(std::string blockid, std::string blockname, std::string file, int line)
{
	m_blockid	= blockid;	
	m_blockname	= blockname;
	m_line = line;
	m_file = file;
}

RFSMBlockInfo::~RFSMBlockInfo(void)
{
}

void RFSMBlockInfo::setBlockName(std::string blockname)
{
	m_blockname = blockname;
}

void RFSMBlockInfo::setBlockID(std::string blockpath)
{
	m_blockid = blockpath;
}

std::string RFSMBlockInfo::getBlockName()
{
	return m_blockname;
}

std::string RFSMBlockInfo::getBlockID()
{
	return m_blockid;
}

void RFSMBlockInfo::setEndLine(int line)
{
	m_line = line;
}

int RFSMBlockInfo::getEndLine()
{
	return m_line;
}

std::string RFSMBlockInfo::getFileName()
{
	return m_file;
}