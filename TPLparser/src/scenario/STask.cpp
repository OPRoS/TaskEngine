/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2011.08.16
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "scenario/STask.h"

STask::STask(void)
{
}

STask::~STask(void)
{
}

std::string STask::getName(void)
{
	return m_name;
}

void STask::setName(std::string name)
{
	m_name = name;
}

void STask::setStart(std::string start)
{
	m_start = start;
}

std::string STask::getStart(void)
{
	return m_start;
}