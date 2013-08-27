/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once
//#include "common/Ginclude.h"
#include "stxmodel/GmodelVar.h"
#include "stxmodel/Gfunction.h"

class Gmodel
{
private:
	Gtoken*				m_name;
	EList<GmodelVar*>*	m_varList;
	EList<Gmodel*>*		m_modelList;
	EList<Gfunction*>*  m_funcList;

public:
	Gmodel(void);
	virtual ~Gmodel(void);

	void addVariable(GmodelVar*);
	void addModel(Gmodel*);
	void addFunc(Gfunction*);

	void setName(Gtoken*);
	Gtoken* getName();
	EList<GmodelVar*>* getVariables();
	EList<Gmodel*>* getModels();
	EList<Gfunction*>* getFunctions();
};
