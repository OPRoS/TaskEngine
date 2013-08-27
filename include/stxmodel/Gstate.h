/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include <iostream>
#include "stxmodel/GstateTrans.h"
#include "stxmodel/GstmtBlock.h"
#include "stxmodel/GstateDo.h"
#include "stxmodel/GstateEntry.h"
#include "stxmodel/GstateExit.h"
#include "stxmodel/Gtoken.h"
#include "stxmodel/GlocalVar.h"
#include <vector>
#include <algorithm>
//#include "HasBlock.h"

/**
This class corresponds to State in Behavior
*/

class Gstate : public GstmtBlock
{
private:	
	int				m_mod; //initial, goal
	Gtoken*			m_name;	

	//EList<Gstmt*>*	m_stateVarList;
	GstateTrans*	m_transition;
	GstateDo*		m_do;
	GstateEntry*	m_entry;
	GstateExit*		m_exit;

	//std::vector<int>	m_synchList;

	void setDo(GstateDo*);
	void setEntry(GstateEntry*);
	void setExit(GstateExit*);

public:
	//Gstate(std::string file, int eol, int blockid, std::string blockname, std::string blockpath);
	Gstate(std::string file, int blockid);
	virtual ~Gstate(void);

	void	print(unsigned int);
	char*	toString(){return NULL;};

	void setModifier(int);
	void setName(Gtoken*);
	void setTransition(GstateTrans*);
	void setActionBlock(GstmtBlock*);
	//void setStateVars(EList<Gstmt*>* vars);
	void addStateVar(GlocalVar* lvar);
	//void addSynchID(int);
	//void copySynchIDs(std::vector<int>&);

	int					getModifier();
	Gtoken*				getName();
	GstateTrans*		getTransition();
	GstateDo*			getDoBlock();
	GstateEntry*		getEntryBlock();
	GstateExit*			getExitBlock();
	EList<Gstmt*>*		getStateVars();
	
};
