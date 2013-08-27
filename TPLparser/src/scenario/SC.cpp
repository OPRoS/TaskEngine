/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2011.08.16
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "scenario/SC.h"

SC::SC(void)
{
}

SC::~SC(void)
{
	if(bslist.size() > 0){
		for(unsigned int i = 0; i < bslist.size(); i++){
			delete bslist[i];
		}
		bslist.clear();
	}
}

std::string SC::getSCname()
{
	return m_scname;
}

std::vector<SBhvSeq*> SC::getBSlist()
{
	return bslist;
}

void SC::addBS(SBhvSeq* bs)
{
	bslist.push_back(bs);
}

void SC::setName(std::string scname)
{
	m_scname = scname;
}

void SC::setType(std::string tystr)
{
	m_type = tystr;
}

std::string SC::getType(void)
{
	return m_type;
}