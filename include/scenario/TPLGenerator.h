/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Scenario 
** Date:        2011.08.15
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include <set>
#include "scenario/SEvent.h"
#include "scenario/SBehavior.h"
#include "scenario/SC.h"
#include "scenario/STask.h"
#include "scenario/NodeBS.h"
#include "scenario/NodeBhv.h"
#include "scenario/NodeSeq.h"
#include "scenario/NodeCon.h"
#include "scenario/FSM4BS.h"
#include "codegen/CodeWriter.h"
#include "common/Gdatatype.h"
#include "stxmodel/Gtoken.h"
#include "log/Log.h"

class TPLGenerator :public CodeWriter
{
private:
	Log				m_log;
	std::string		m_scfname;
	std::string		m_inclStr;
	std::string		m_modelname;

	std::map<std::string, std::string*>				m_bhvTab;
	std::map<std::string, std::string*>::iterator	m_btIter;
	typedef std::pair<std::string, std::string*>	pstrpair;

	std::map<std::string, std::vector<std::string>*>				m_epdlist;
	std::map<std::string, std::vector<std::string>*>::iterator	m_epdlistIter;
	typedef std::pair<std::string, std::vector<std::string>*>	m_epdlistPair;

	std::map<std::string, std::set<std::string>>::iterator	m_duplistIter;
	typedef std::pair<std::string, std::set<std::string>>	m_duplistPair;

	std::set<std::string> m_cycleBhvs;
	std::set<std::string> m_modelvars;

	std::vector<STask*>		m_stasks;
	std::vector<SC*>		m_sclist;
	std::vector<SBehavior*> m_sbhvlist;
	std::vector<SEvent*>	m_sevtlist;

	//-------------------------------------------------------
	std::map<std::string, FSM4BS*>				m_fsmTAB;
	std::map<std::string, FSM4BS*>::iterator	m_fsmTABIter;
	typedef std::pair<std::string, FSM4BS*>		m_fsmTABPair;
	//-------------------------------------------------------
	
public:
	TPLGenerator(OSFileSys*, std::string, std::string, std::vector<STask*>, std::vector<SC*>, std::vector<SBehavior*>, std::vector<SEvent*>);
	virtual ~TPLGenerator(void);

	void generateTPL	(std::string styleFile);
	//void findConnection	(NodeBS*, NodeBS*, std::string);
	void initBhvTable	(NodeBS*, std::string);
	//void resetBhvTable	(void);
	void detectCycle	(NodeBS*, std::vector<std::string>);
	bool isCycled		(std::string);
	bool isLeaf			(NodeBS* node);
	//void addEpdlist		(std::string nodename, std::string epd);
	//void buildUpExpands	(void);

	std::string getBehaviorActions	(std::string);
	std::string getEventExpr		(std::string);

	void addEvtNode4Monitor	(NodeBhv*);
	void makeFSM			(NodeBS*, std::string);
	void makeFSM4ExptMon	();
	void findDuplicatedBhv	(NodeBS*, std::set<std::string>&, std::map<std::string, std::set<std::string>>&);
	void addStates4Bhvs		(std::map<std::string, std::set<std::string>>&, FSM4BS* fsm, NodeBS* parent, std::string baname, NodeBS* cur/*, std::string bsRef*/);
	//void postProc4FSM		(std::set<std::string>&, FSM4BS* fsm, NodeBS* ps, NodeBS* bs, std::string bsRef2);
	void setPrefix			(std::string, NodeBS*);
	void addStates4PostCon	(FSM4BS* fsm);
	bool hasNestSeq			(std::string prefix);
	bool isSCname			(std::string stateId);
	bool isIn				(std::string, std::set<std::string>&);
	//bool isState4ExptMon	(NodeState*);
	//bool isFirstBS		(std::string bsid);
	void generateFSMCode		(FILE*, std::string, FSM4BS*);
	void generateFSMCode2		(FILE*, std::string, FSM4BS*);
	void generateStartBhvCode	(FILE*, std::string, FSM4BS*);
	void generateModelCode		(FILE*);
	std::string getTransBlock	(FSM4BS*, NodeState*);
	std::string getState4Trans	(FSM4BS*, NodeState*);
	std::string getMonitorVars	(NodeState*);

	
	//std::vector<std::string> getBSstates(std::string stateid);
	//std::string getCond4NotBS(std::vector<std::string>);
	std::string getNewPrefix			(std::string, NodeBS*);
	std::string getMonPostfix			(NodeBhv* bhv);
	std::string getStateUName4ExpandBhv	(NodeBhv*);
	std::string getStartStateOfBS		(std::vector<SBhvSeq*>, std::string stateId);
	std::string attachModelname			(std::string);
	NodeBhv*	findFirstBhvNode		(NodeBS* node);
	NodeTrans*	buildTrans				(NodeState* sourceState, NodeState* targetState, std::string evtRef);

	void updateDuplicatedList	(NodeBhv*, std::set<std::string>&, std::map<std::string, std::set<std::string>>&);
	int	 getNumOfPrefix			(std::string bhv, std::map<std::string, std::set<std::string>>& duplicatedList);
	void addStates4Branch		(FSM4BS* fsm);
	void addInitialState		(FSM4BS* fsm);
};
