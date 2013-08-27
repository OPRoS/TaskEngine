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

class Genum
{
private:
	Gtoken*				m_name;
	EList<Gtoken*>*		m_elist;

public:
	Genum(void);
	virtual ~Genum(void);

	void addElement(Gtoken*);
	void setName(Gtoken*);
	Gtoken* getName();
	EList<Gtoken*>* getElements();
};
