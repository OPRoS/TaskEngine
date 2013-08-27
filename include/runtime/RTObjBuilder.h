/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include <typeinfo>
#include "interpreter/EParser.h"
#include "common/GerrorHandler.h"
#include "runtime/symtab/SymTAB4Task.h"
#include "runtime/symtab/TABcall.h"
#include "runtime/rtmodel/RFSMliteral.h"
#include "runtime/rtmodel/RFSMRelational.h"
#include "runtime/rtmodel/RFSMcondValue.h"
#include "runtime/rtmodel/RFSMadd.h"
#include "runtime/rtmodel/RFSMmulti.h"
#include "runtime/rtmodel/RFSMasmt.h"
#include "runtime/rtmodel/RFSMboolable.h"
#include "runtime/rtmodel/RFSMand.h"
#include "runtime/rtmodel/RFSMor.h"
#include "runtime/rtmodel/RFSMcallFunc.h"
#include "runtime/rtmodel/RFSMvariable.h"
#include "runtime/rtmodel/RFSMbehavior.h"
#include "runtime/rtmodel/RFSMconnector.h"
#include "runtime/rtmodel/RFSMtask.h"
#include "runtime/rtmodel/RFSMconnector.h"
#include "runtime/rtmodel/RFSMtransition.h"
#include "runtime/rtmodel/RFSMstmt.h"
#include "runtime/rtmodel/RFSMgotoStmt.h"
#include "runtime/rtmodel/RFSMifStmt.h"
#include "runtime/rtmodel/RFSMcallAction.h"
#include "runtime/rtmodel/RFSMcallBhv.h"
#include "runtime/rtmodel/RFSMcallConnector.h"
#include "runtime/rtmodel/RFSMsign.h"
#include "runtime/rtmodel/RFSMsysWaitfor.h"
#include "runtime/rtmodel/RFSMsysSynch.h"
#include "runtime/rtmodel/RFSMenum.h"
#include "runtime/rtmodel/RFSMsystem.h"
#include "runtime/rtmodel/RFSMStmtBlock.h"
#include "runtime/rtmodel/RFSMsymb.h"
#include "runtime/rtmodel/RFSMvar.h"

//#pragma warning (disable : 4290)

class RTObjBuilder
{
private:
	SymTAB4Task*	m_symT;
	RFSMtreenode*	m_curTreeNode;		//여러 Behavior를 변환하면서, 현재 처리하는 Task이름, delete에서 지우면 안된다.
	RFSMstate*		m_curState;		    //현재 변환하는 Behavior의 현재 State이름
	std::ostream*	m_ostr;	

public:
	RTObjBuilder(std::ostream*, SymTAB4Task*);
	virtual ~RTObjBuilder(void);

	/* symbol 테이블 */
	EList<TABparameter*>* makeTABparams(EList<Gparameter*>* pList);
	void makeSymTable(GparsedResult*) throw (ErrorHandler&);
	void makeSymT(ErrorHandler&, EList<Gmodel*>*, std::string);
	void makeSymT(ErrorHandler&, EList<Genum*>*, std::string);
	void makeSymT(ErrorHandler&, EList<Gaction*>*, std::string);
	void makeSymT(ErrorHandler&, EList<Gbehavior*>*, std::string);
	void makeSymT(ErrorHandler&, EList<Gconnector*>*, std::string);
	

	/* semantics검사 */
	EList<RFSMtask*>*		checkSemantics		(EList<Gtask*>*)		throw(ErrorHandler&);
	EList<RFSMbehavior*>*	checkSemantics		(EList<Gbehavior*>*)	throw(ErrorHandler&);
	EList<RFSMconnector*>*	checkSemantics		(EList<Gconnector*>*)	throw(ErrorHandler&);
	RFSMtask*				checktask			(ErrorHandler&, Gtask*);
	RFSMbehavior*			checkBehavior		(ErrorHandler&, Gbehavior*);
	RFSMconnector*			checkConnector		(ErrorHandler&, Gconnector*);
	void					checkBhvConnParams	(ErrorHandler&, EList<Gparameter*>*);//선언문
	void					checkVarsBlock		(ErrorHandler&, EList<Gstmt*>*, RFSMcst*);
	//RFSMtransition*		checkTransition(ErrorHandler& eHandler, GstateTrans* trans, const std::string blockpath);

//EList<RFSMstmt*>* checkTransition(ErrorHandler& eHandler, GstmtBlock *stmtBlock, const std::string blockpath);

	std::vector<RFSMstmt*>*	checkActionStmts	(ErrorHandler&, EList<Gstmt*>*);
	std::vector<RFSMstmt*>*	checkTransStmts		(ErrorHandler&, EList<Gstmt*>*, bool);
	RFSMstmt*				checkActionStmt		(ErrorHandler&, Gstmt*);
	RFSMstmt*				checkTransStmt		(ErrorHandler&, Gstmt*, bool);
	RFSMstmt*				checkCommonStmts	(ErrorHandler&, Gstmt*, bool isRunBlock);
	void					checkDeterminism	(ErrorHandler&, Gstmt*, int&);
	//RFSMstmt*			checkStmt(ErrorHandler& eHandler, Gstmt* stmt);

	//statement타입=>value타입의 assignment도 statement타입
	RFSMifStmt*			checkIfstmt			(ErrorHandler&, GstmtIf*, bool inTrans, bool isRunBlock);
	RFSMcallBhv*		checkBhvCallstmt	(ErrorHandler&, GstmtInvokeSub*, bool isRunBlock);
	RFSMcallConnector*	checkConnCallstmt	(ErrorHandler&, GstmtInvokeSub*, bool);
	RFSMcallFunc*		checkFuncCallstmt	(ErrorHandler&, GstmtCall*);
	RFSMgotoStmt*		checkGoto			(ErrorHandler&, Gtoken*);
	RFSMsysWaitfor*		checkWaitfor		(ErrorHandler&, Gwaitfor*);
	RFSMsysSynch*		checkSynch			(ErrorHandler&, Gsynch*);
	RFSMstmt*			checkLocalVarDecl	(ErrorHandler&, GlocalVar*);
	void				checkFuncParamsDecl	(ErrorHandler&, EList<Gparameter*>*);//선언문
	void				checkFuncParameter	(ErrorHandler&, Gparameter*);
	void				checkVariableType	(ErrorHandler&, Gtoken* type);
	void				checkCallParam		(ErrorHandler&, Gtoken*, EXTree*, TABcall*, RFSMcall*);//호출문
	void				checkCallParams		(ErrorHandler&, Gtoken*, EList<GstmtExpr*>*, TABcall*, RFSMcall*);
	//void				checkStmt(ErrorHandler& eHandler, Gstmt* stmt);

	//bool타입
	RFSMboolable*	checkLOGICALExpr(ErrorHandler& eHandler, EXTree* expr);
	RFSMRelational* checkRelExpr	(ErrorHandler& eHandler, EXTree* expr);
	RFSMand*		checkANDExpr	(ErrorHandler& eHandler, EXTree* expr);
	RFSMor*			checkORExpr		(ErrorHandler& eHandler, EXTree* expr);

	//value 타입
	RFSMvalue*		checkCPARAMExpr	(ErrorHandler& eHandler, EXTree* expr);
	RFSMvalue*		checkVALUEExpr	(ErrorHandler& eHandler, EXTree* expr);
	RFSMadd*		checkADDExpr	(ErrorHandler& eHandler, EXTree* expr);
	RFSMmulti*		checkMULExpr	(ErrorHandler& eHandler, EXTree* expr);
	RFSMsign*		checkSIGNExpr	(ErrorHandler& eHandler, EXTree* expr);
	RFSMasmt*		checkASMTExpr	(ErrorHandler& eHandler, EXTree* expr);
	RFSMcondValue*	checkCONDExpr	(ErrorHandler& eHandler, EXTree* expr);
	RFSMsym*		checkSYMBExpr	(ErrorHandler& eHandler, EXTree* expr);
	RFSMliteral*	checkLITEExpr	(ErrorHandler& eHandler, EXTree* expr);
	RFSMcallFunc*	checkFUNCExpr	(ErrorHandler& eHandler, EXTree* expr);


	void printWarningMsg(int exceptionType, Gtoken*);
	void printErrMsg(int exceptionType, Gtoken*);

	SymTAB4Task* getSymT(void);
};

typedef unsigned long (*TimeFunction)();
