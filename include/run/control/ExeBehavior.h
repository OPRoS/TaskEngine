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
#include <time.h>
#include "run/control/ExeTask.h"
#include "run/control/PerformTable.h"
#include "run/control/ExeTaskTreeNode.h"
#include "runtime/rtmodel/RFSMnamedItem.h"
#include "common/TypeDef.h"
#include "util/MultiOS.h"
#include "ds/EList.h"
#include "log/LogFile.h"

class ExeBehavior: public ExeTaskTreeNode
{
private:
	//std::string	m_name;
	std::vector<std::string>	m_goalStates;
	std::string		m_curState;
	//std::string   m_blockID;
	unsigned int	m_curStateBlock;	//entry(1), transition(2), stay(3), exit(4)

	bool			m_isCurStateEntered;
	bool			m_isCurStateExited;

	ValueMap		m_params;

	struct timeb	m_curStateStart;
	long			m_iterCount; //root behavior만 해당

public:	
	ExeBehavior(RemoteCLI*, LogFile*, ExeTaskTreeNode*, std::string name, int nameLine, std::vector<std::string> goalstates, std::string blockID, std::string filename, int runType, int callstmtLine, unsigned int);
	virtual ~ExeBehavior(void);

	void setCurState		(std::string);
	void setCurStateBlock	(unsigned int);	
	void stateStart			(void);
	
	std::string 		getCurState		(void);
	unsigned int		getCurStateBlock(void);
	//std::string 		getBlockID();	
	RFSMvalueEval 		getStateTime	(int);
	long				getIterCount	(void);

	bool isOnGoalState	(void);
	void entryExecuted	(void);
	bool isEntryExecuted(void);
	void exitExecuted	(void);
	bool isExitExecuted	(void);

	/*RThread 함수 재정의*/
	//virtual int stop();	
	virtual void	cbackBy_Thrfunc	(void);
	virtual	int		resume			(void);

	int		execute		(void);
	void	setCurParam	(ValueMap);
#ifdef _WIN32
	virtual void	cbackBy_Timerfunc(PVOID param, bool timerOrWaitFired);
#endif
};

const std::string STATE_TIME = std::string("state_time");
const unsigned int ENTRY_BLOCK	= 1;
const unsigned int TRANS_BLOCK	= 2;
const unsigned int STAY_BLOCK	= 3;
const unsigned int EXIT_BLOCK	= 4;


