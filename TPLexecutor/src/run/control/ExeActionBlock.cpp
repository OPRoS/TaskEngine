/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "run/control/ExeActionBlock.h"

ExeActionBlock::ExeActionBlock(ExeTask* eTask, unsigned int tp): RThread("ExeActionBlock", tp)
{
	m_runner		= eTask->getController();
	m_wVarTAB	= new ValueMap();
}

void ExeActionBlock::setParameters(std::vector<RFSMstmt*>* stmts, ExeBehavior* ebhv)
{
	m_stmts = stmts;
	m_ebhv = ebhv;
}

ExeActionBlock:: ~ExeActionBlock(void)
{
	m_wVarTAB->clear();
	delete m_wVarTAB;
}


int ExeActionBlock::execute()
{
/*
	//���࿡ �ʿ��� �½�ũ ����Ʈ����
	RFSMbehavior* task = getBehavior(m_startBehavior);

	if(task == NULL){

		//return ����
	}

	//�����½�ũ ����
	m_startExeBehavior = new ExeBehavior(m_logFile, m_startBehavior, task->getBlockID());
	std::string isname = task->getInitialStateName();
	m_startExeBehavior->setCurState(isname);//state_time�� ���⼭ ����
*/
	//exeBehavior(m_startExeBehavior, NULL);
	
	this->create_thr(RThread::ATTACH, "ACTION_BLOCK");
	return 0;
}

void ExeActionBlock::cbackBy_Thrfunc()
{
	m_running = true;
	m_runner->enterActionBlock(m_stmts, m_ebhv);
}
