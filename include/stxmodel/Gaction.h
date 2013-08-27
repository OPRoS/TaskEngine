/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "stxmodel/Ggram.h"
#include "stxmodel/Gparameter.h"
#include "ds/EList.h"

class Gaction : public Ggram
{
private:
	Gtoken*				m_name;
	EList<Gparameter*>* m_params;

public:
	Gaction(std::string, int eol, std::string, std::string);
	virtual ~Gaction(void);

	/* set */
	void setName		(Gtoken*);
	void setParameters	(EList<Gparameter*>*);
	void addParameter	(Gparameter*);

	/* get */
	EList<Gparameter*>* getParameters();
	Gtoken*				getName();

	char* toString(){return NULL;};
	void print(unsigned int);
};
