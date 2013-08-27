/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include <string>
#include "Gtoken.h"
#include "GlocalVar.h"

class GmodelVar : public GlocalVar
{
private:
	unsigned int	m_mode; //IN , OUT, GVAR, WVAR
	
public:
	GmodelVar(unsigned int, Gtoken*, Gtoken*, std::string file, int, std::string, std::string);
	GmodelVar(std::string file, int eol, std::string blockname, std::string blockpath);
	virtual ~GmodelVar(void);

	void setMode(unsigned int);	
	unsigned int	getMode();

	GmodelVar		deepCopy();
};
