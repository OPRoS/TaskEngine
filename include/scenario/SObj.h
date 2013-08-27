/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Scenario 
** Date:        2011.08.15
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "stxmodel/Gtoken.h"

class SObj
{
private:
	Gtoken*	m_name;
	Gtoken*	m_desc;
	int		m_eol;

public:
	SObj(void);
	virtual ~SObj(void);

	void setName(Gtoken*);
	void setDesc(Gtoken*);
	void setEOL(int);
	Gtoken*	getName();
	Gtoken* getExpr();
	int		getEOL();
};
