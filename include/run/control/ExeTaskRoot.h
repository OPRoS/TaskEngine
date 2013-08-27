/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "log/LogFile.h"
#include "run/control/TaskRunner.h"
#include "run/control/ExeTaskTreeNode.h"

class ExeTaskRoot : public RThread
{
private:
	ExeTask*	m_eTask;
	bool		m_runCompleted;

public:
	ExeTaskRoot(ExeTask*, unsigned int);
	virtual ~ExeTaskRoot(void);
	
	//RThread
	virtual void cbackBy_Thrfunc(void);
	int			execute		(void);
	ExeTask*	getExeTask	(void);
};



