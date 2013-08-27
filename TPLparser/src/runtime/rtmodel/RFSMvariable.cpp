/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMvariable.h"

RFSMvariable::RFSMvariable(std::string blockPath, std::string blockname, std::string name, unsigned int type, unsigned int mode, std::string file, int line)
             :RFSMsym(blockPath, blockname, name, file, line), RFSMboolable(CLS_RFSMvariable)
{
	//m_blockPath = blockPath;
	//m_blockID = blockID;
	//m_blockName = blockname;
	m_type		= type;
	m_varMode	= mode;
}

RFSMvariable:: ~RFSMvariable(void)
{
}

/*
std::string RFSMvariable::getBlockPath()
{
	return m_blockPath;
}

std::string RFSMvariable::getBlockID()
{
	return m_blockID;
}

std::string RFSMvariable::getBlockName()
{
	return m_blockName;
}
*/

unsigned int RFSMvariable::getType()
{
	return m_type;
}

unsigned int RFSMvariable::getMode()
{
	return m_varMode;
}

std::string RFSMvariable::toString()
{
	return this->getName();
}
