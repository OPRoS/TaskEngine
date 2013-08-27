/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once


#include "util/Utility.h"
#include "common/GerrorHandler.h"
#include "common/antlrIncl.h"
#include "common/Gdatatype.h"
#include "stxmodel/Gtask.h"
#include "stxmodel/Ginclude.h"
#include "stxmodel/Gbehavior.h"
#include "stxmodel/Gstate.h"
#include "stxmodel/Gtoken.h"
#include "stxmodel/Gparameter.h"
#include "stxmodel/GstateTrans.h"
#include "stxmodel/GstmtExpr.h"
#include "stxmodel/GstmtGotoState.h"
#include "stxmodel/GstmtInvokeSub.h"
#include "stxmodel/GstmtCall.h"
#include "stxmodel/Genum.h"
#include "stxmodel/Gmodel.h"
#include "stxmodel/Gcontext.h"
#include "stxmodel/Grule.h"
#include "stxmodel/Gaction.h"
#include "stxmodel/Gfunction.h"
#include "stxmodel/Gworker.h"
#include "stxmodel/Gsynch.h"
#include "stxmodel/Gwaitfor.h"
#include "stxmodel/GparsedResult.h"
#include "stxmodel/Gconnector.h"
//#include "runtime/TaskConfig.h"
#include "scenario/SEvent.h"
#include "scenario/SBehavior.h"
#include "scenario/SBhvSeq.h"
#include "scenario/NodeBS.h"
#include "scenario/NodeBhv.h"
#include "scenario/NodeSeq.h"
#include "scenario/NodeCon.h"
#include "log/Log.h"
#include "common/EXTree.h"
#include <map>
#include <iostream>
#include <string>
#include <fstream>
#include <memory>
#include "util/Utility.h"

//#include "runtime/rtmodel/RFSMsemantics.h"

/**
	@class EParser 
*/
class EParser
{
private:
	int	BID;
	int SNodeID;	//시나리오의 behavior 식별자(사이클 처리를 위해서필요)
	Log log;

	std::string			m_curfName;
	std::string			m_curWorkingNode;	//현재 파싱하는 Gbehavior,Gconnector 또는 Gtask
	std::string			m_curWorkingState;
	std::string			m_curWorkingBlock;	//entry, transition, stay, exit, var, construct, destruct
	std::string			m_curWorkingBlockStmt; //with, parellel, sequential

	GparsedResult*		m_parsedResult;
	Ginclude*			m_incl; //preprocessing 동안에 찾아진 include;

	std::string			getCurBlockFullName(void);
	EList<Gstate*>*		getStates		(const std::string, pANTLR3_BASE_TREE);
	Gstate*				getState		(const std::string, pANTLR3_BASE_TREE);
	EList<Gparameter*>*	getParameters	(const std::string, pANTLR3_BASE_TREE);
	Gparameter*			getParameter	(const std::string, pANTLR3_BASE_TREE);
	GstateTrans*		getTransition	(const std::string, pANTLR3_BASE_TREE);
	GstmtBlock*			getRunblock		(const std::string, pANTLR3_BASE_TREE);
	GstmtBlock*			getStmtBlock	(const std::string, pANTLR3_BASE_TREE);
	GstmtBlock*			getActionBlock	(const std::string, pANTLR3_BASE_TREE);
	GstmtBlock*			getConstruct	(const std::string, pANTLR3_BASE_TREE);
	GstmtBlock*			getDestruct		(const std::string, pANTLR3_BASE_TREE);
	GstmtIf*			getIfstmt		(const std::string, pANTLR3_BASE_TREE);
	EList<Gstmt*>*		getStmts		(const std::string, pANTLR3_BASE_TREE);
	Gstmt*				getStmt			(const std::string, pANTLR3_BASE_TREE);
	GlocalVar*			getLocalVarDecl	(const std::string, pANTLR3_BASE_TREE);
	Gwaitfor*			getWait			(const std::string, pANTLR3_BASE_TREE);
	Gsynch*				getSynch		(const std::string, pANTLR3_BASE_TREE);
	GstmtExpr*			getExpr			(const std::string, pANTLR3_BASE_TREE);
	GstmtGotoState*		getGotoState	(const std::string, pANTLR3_BASE_TREE);
	GstmtInvokeSub*		getInvokeSub	(const std::string, pANTLR3_BASE_TREE);
	Gcall*				getFuncall		(const std::string, pANTLR3_BASE_TREE);
	GstmtCall*			getCallStmt		(const std::string, pANTLR3_BASE_TREE);
	EList<GstmtExpr*>*	getCallParams	(const std::string, pANTLR3_BASE_TREE);
	EXTree*				getExprTree		(const std::string, pANTLR3_BASE_TREE);

	Gtoken*				getSymbol		(pANTLR3_BASE_TREE);
	GmodelVar*			getModelVar		(pANTLR3_BASE_TREE);
	Gparameter*         getActionParam	(pANTLR3_BASE_TREE);
	Gaction*			getAction		(pANTLR3_BASE_TREE);
	Gfunction*			getModelFunc	(pANTLR3_BASE_TREE);
	//Gworker*			getWorker		(pANTLR3_BASE_TREE);
	//Gworker*			getWorker2		(pANTLR3_BASE_TREE);
	Gtask*				getTask			(pANTLR3_BASE_TREE);

	std::string			symTree3str		(pANTLR3_BASE_TREE);
	//void				setInclude		(pANTLR3_BASE_TREE, Ginclude*);
	//void				updateInclude	(pANTLR3_BASE_TREE, Ginclude*);
	//void				makeExprTree	(const std::string blockpath, EXTree*, pANTLR3_BASE_TREE);
	void				makeExprTree	(const std::string blockpath, EXTree*, pANTLR3_BASE_TREE);
	void				buildUpBehavior	(const std::string blockpath, pANTLR3_BASE_TREE, Gbehavior*);
	void				buildUpConnector(const std::string blockpath, pANTLR3_BASE_TREE, Gconnector*);
	void				buildUpVarBlock	(const std::string blockpath, pANTLR3_BASE_TREE, GstmtBlock*);
	void				buildUpConBlock	(const std::string blockpath, pANTLR3_BASE_TREE, Gconnector*);
	void				setPeriod		(Gstmt* stmt, int period);

	//시나리오
	void				buildUpSEvents		(pANTLR3_BASE_TREE);
	void				buildUpSBehaviors	(pANTLR3_BASE_TREE);
	void				buildUpSC			(pANTLR3_BASE_TREE);
	void				buildUpSTask		(pANTLR3_BASE_TREE);
	SEvent*				getSEvent			(pANTLR3_BASE_TREE);
	SBehavior*			getSBehavior		(pANTLR3_BASE_TREE);
	SBhvSeq*			getSBhvSeq			(pANTLR3_BASE_TREE);
	NodeBS*				getNodeBS			(pANTLR3_BASE_TREE);
	NodeBS*				getNodeSeq			(pANTLR3_BASE_TREE);
	NodeBS*				getNodeCon			(pANTLR3_BASE_TREE);

	//파싱결과가 궁금할 때...아래 유형을 이용하자.. (여기쓴이유는 잘보이라고..ㅋㅋ)
	//printf((const char*)varTree->toStringTree(varTree)->chars);

public:

	EParser(std::ostream*);
	virtual ~EParser(void);

	double	circle_area(float);

	void	navigator	(pANTLR3_BASE_TREE tree, int indent);
	bool	isStepping	(pANTLR3_BASE_TREE tree);
	bool	isPrimitive	(pANTLR3_BASE_TREE tree);
	bool    isNOTsym	(pANTLR3_BASE_TREE tree);

	int		parseScript(std::string);
	void	setFilename(std::string);

	int				extractInclude	(std::string filename, std::string deployHome);
	int				parseDef2		(pANTLR3_BASE_TREE);
	Genum*			getEnum			(pANTLR3_BASE_TREE);
	Gmodel* 		getModel		(pANTLR3_BASE_TREE);
	Gcontext* 		getContext		(pANTLR3_BASE_TREE);
	//Grule*		getRule			(pANTLR3_BASE_TREE);
	Ginclude*		getInclude		(void);
	GparsedResult*	getParsingResult();
	//int exprTest(std::string);

	int parseStrBlock(std::string exprString, int type, SObj*, std::string);
};
