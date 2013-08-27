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
#include "stxmodel/Ggram.h"
#include "stxmodel/Gtoken.h"

class Gparameter
{
private:
	bool	m_isPrim;
	Gtoken	*m_type;
	Gtoken	*m_name;
	Gtoken	*m_value;

public:
	Gparameter(void);
	virtual ~Gparameter(void);

	void print(unsigned int);
	char* toString(){return NULL;};
	
	void setType(Gtoken *type);
	void setName(Gtoken *name);
	void setValue(Gtoken *value);
	void setAsPrim();
	
	Gtoken*	getType();
	Gtoken*	getName();
	Gtoken*	getValue();

};
