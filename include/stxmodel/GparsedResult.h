/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

//#include "EList.h"
#include "stxmodel/Genum.h"
#include "stxmodel/Gmodel.h"
#include "stxmodel/Gaction.h"
#include "stxmodel/Gbehavior.h"
#include "stxmodel/Gworker.h"
#include "stxmodel/Gtask.h"
#include "stxmodel/Gcontext.h"
#include "stxmodel/Gconnector.h"
#include "scenario/SEvent.h"
#include "scenario/SBehavior.h"
#include "scenario/SBhvSeq.h"
#include "scenario/SC.h"
#include "scenario/STask.h"
#include <vector>

class GparsedResult
{
private:
//	Ginclude*			m_incl;
	EList<Genum*>*		m_enumlist;
	EList<Gmodel*>*		m_modellist;
	EList<Gcontext*>*	m_ctxlist;
	EList<Gaction*>*	m_actionlist;
	EList<Gtask*>*		m_tasklist;
	EList<Gbehavior*>*	m_bhvlist;
	EList<Gconnector*>* m_conlist;

	std::vector<SEvent*>	m_sevents;
	std::vector<SBehavior*>	m_sbhvs;
	//std::vector<SBhvSeq*>	m_bhvseq;
	std::vector<SC*>		m_sclist;
	std::vector<STask*>		m_stasks;

public:
	GparsedResult(void);
	virtual ~GparsedResult(void);

	void addEnum		(Genum*);
	void addModel		(Gmodel*);
	void addContext		(Gcontext*);
	void addAction		(Gaction*);
	void addTask		(Gtask*);
	void addbehavior	(Gbehavior*);
	void addconnector	(Gconnector*);
	
	void addSEvent		(SEvent*);
	void addSBehavior	(SBehavior*);
	void addSC			(SC*);
	void addSTask		(STask*);

//	void setInclude(Ginclude*);
	EList<Genum*>*		getEnums	(void);
	EList<Gmodel*>*		getModels	(void);
	EList<Gcontext*>*	getContexts	(void);
	EList<Gaction*>*	getActions	(void);
	EList<Gtask*>*		getTasks	(void);
	EList<Gbehavior*>*	getbehaviors(void);
	EList<Gconnector*>* getconnectors(void);

	std::vector<SEvent*>	getSEvents		(void);
	std::vector<SBehavior*>	getSBehaviors	(void);
	std::vector<SC*>		getSCs			(void);
	std::vector<STask*>		getSTasks		(void);
};
