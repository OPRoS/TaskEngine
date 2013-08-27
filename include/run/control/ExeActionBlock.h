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

	ValueMap*			m_wVarTAB;		//worker ���� �����
	ValueMap			m_valueTAB;		//�ϳ��� �½�ũ���� ���Ǵ� ������ state_time, taks_time, ��Ÿ ����������
										//���� �����? ����ٰ� �ٽ� ������: ���Ź���

public:

	ExeActionBlock(ExeTask* eTask, unsigned int);
	virtual ~ExeActionBlock(void);

	void setParameters(std::vector<RFSMstmt*>* stmts, ExeBehavior* ebhv);

	//Thread ������ ���ؼ� �ʿ��� �Լ�
	int execute();	
	virtual void cbackBy_Thrfunc();
};
