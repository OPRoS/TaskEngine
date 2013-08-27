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
#include "runtime/rtmodel/RFSMnamedItem.h"
#include "runtime/rtmodel/RFSMasmt.h"
#include "binding/ModelBinder.h"
#include "common/TypeDef.h"
#include "thread/RThread.h"
#include "ds/EList.h"
#include "run/control/TaskRunner.h"
#include "run/control/ExeTask.h"
#include "run/control/ExeBehavior.h"

class TaskRunner;
class ExeBehavior;

class ExeActionBlock: public RThread
{
private:
	LogFile*			m_logFile;
	TaskRunner*			m_runner;

	std::vector<RFSMstmt*>*	m_stmts;
	ExeBehavior*			m_ebhv;

	ValueMap*			m_wVarTAB;		//worker 값의 저장소
	ValueMap			m_valueTAB;		//하나의 태스크에서 사용되는 변수들 state_time, taks_time, 기타 블럭내변수들
										//블럭을 벗어나면? 벗어났다가 다시 들어오면: 갱신문제

public:

	ExeActionBlock(ExeTask* eTask, unsigned int);
	virtual ~ExeActionBlock(void);

	void setParameters(std::vector<RFSMstmt*>* stmts, ExeBehavior* ebhv);

	//Thread 지원을 위해서 필요한 함수
	int execute();	
	virtual void cbackBy_Thrfunc();
};
