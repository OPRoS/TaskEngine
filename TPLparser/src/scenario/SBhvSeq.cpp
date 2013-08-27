/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2011.08.16
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "scenario/SBhvSeq.h"

SBhvSeq::SBhvSeq(void)
{
	m_bs = NULL;
}

SBhvSeq::~SBhvSeq(void)
{
	if(m_bs != NULL) delete m_bs;
}

void SBhvSeq::setStartNode(NodeBS* bs)
{
	m_bs = bs;
}

NodeBS* SBhvSeq::getStartNode(void)
{
	return m_bs;
}


void SBhvSeq::setGoto(std::string gt)
{
	m_goto = gt;
}
std::string SBhvSeq::getGoto(void)
{
	return m_goto;
}

void SBhvSeq::makeGotoBhv(std::string fsmname)
{
	if(m_goto == "begin"){
		NodeBhv* be = new NodeBhv(m_bs->getNodeId()+1000);//1000개이상의 behavior가 없으면 중복안됨
		be->setBhvRef(fsmname);
		((NodeSeq*)m_bs)->addSub(be);
	}
}