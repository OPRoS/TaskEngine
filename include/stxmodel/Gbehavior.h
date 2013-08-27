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
#include <vector>
#include <map>
#include "stxmodel/Gparameter.h"
#include "stxmodel/Gstate.h"
#include "stxmodel/Gtoken.h"
#include "stxmodel/Ginclude.h"
#include "stxmodel/GcstBlock.h"

class Gbehavior : public GcstBlock
{
private:	
	EList<Gparameter*>*	m_parameters;
	EList<Gstate*>*		m_states;	

public:
	Gbehavior(std::string file, int eol, int blockid, std::string blockname, std::string blockpath);
	virtual ~Gbehavior(void);

	/*
	Realization of Member functions in Ggram */
	void	print			(unsigned int);
	char*	toString		(){return NULL;};
	
	void	setParameters	(EList<Gparameter*>*);
	void	setStates		(EList<Gstate*>*);	
	void	addParameter	(Gparameter*);

	EList<Gparameter*>*		getParameters();
	EList<Gstate*>*			getStates();	
	//std::vector<int>		getSynchInfo();
};
