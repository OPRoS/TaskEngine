#pragma once

#include "runtime/rtmodel/RFSMliteral.h"
#include "runtime/rtmodel/RFSMRelational.h"
#include "runtime/rtmodel/RFSMcondValue.h"
#include "runtime/rtmodel/RFSMadd.h"
#include "runtime/rtmodel/RFSMmulti.h"
#include "runtime/rtmodel/RFSMasmt.h"
#include "runtime/rtmodel/RFSMvar.h"
#include "runtime/rtmodel/RFSMboolable.h"
#include "runtime/rtmodel/RFSMand.h"
#include "runtime/rtmodel/RFSMor.h"
#include "runtime/rtmodel/RFSMcallFunc.h"
#include "runtime/rtmodel/RFSMvariable.h"
#include "runtime/rtmodel/RFSMtask.h"
#include "runtime/rtmodel/RFSMbehavior.h"
#include "runtime/rtmodel/RFSMconnector.h"
#include "runtime/rtmodel/RFSMtransition.h"
#include "runtime/rtmodel/RFSMstmt.h"
#include "runtime/rtmodel/RFSMgotoStmt.h"
#include "runtime/rtmodel/RFSMifStmt.h"
#include "runtime/rtmodel/RFSMcallAction.h"
#include "runtime/rtmodel/RFSMcallBhv.h"
#include "runtime/rtmodel/RFSMcallConnector.h"
#include "runtime/rtmodel/RFSMenum.h"
#include "runtime/rtmodel/RFSMsysWaitfor.h"
#include "runtime/rtmodel/RFSMsysSynch.h"
#include "runtime/rtmodel/RFSMsystem.h"
#include "runtime/rtmodel/RFSMsign.h"
#include "runtime/rtmodel/RFSMbinaryOp.h"
#include "runtime/rtmodel/RFSMStmtBlock.h"

#include "run/control/ExeTransition.h"
#include "run/control/ExeBehavior.h"
#include "run/control/ExeConnector.h"
#include "run/control/ExeTaskTreeNode.h"
#include "run/control/ExeTask.h"
#include "run/control/ExeActionBlock.h"
#include "run/control/DbgTable.h"

#include "log/LogFile.h"
#include "thread/RThread.h"
#include "binding/ModelBinder.h"

class ExeConnector;

class TaskRunner
{
private:
	Log				m_log;	
	LogFile*		m_logFile;
	bool			m_traceThreadON;
	ModelBinder*	m_binder;
	ExeTask*		m_eTask;

	//QueueTimerQueue, WIN
	#ifdef _WIN32
	HANDLE			m_Timer;
	HANDLE			m_TimerQueue;
	#endif

	//goto
	bool	m_exitActionPerformed;

	/*디버깅*/
	int		m_exeMode;		//1= 정상, 2=디버깅, 3=모니터링
	bool	m_suspend;

	ValueMap m_modelTAB;
	std::map<int, bool> m_moveNext_STEP;
	std::map<int, bool> m_moveNext_BP;
	std::map<int, pthread_cond_t>	m_BP_wakeup_conds;  //쓰레드가 생성되고 죽일 때 (삽입 삭제)
	std::map<int, pthread_mutex_t>	m_BP_wakeup_mus;
	pthread_cond_t		m_suspend_cond;
	pthread_mutex_t		m_suspend_mu;


public:
	TaskRunner(ExeTask*, ModelBinder*, LogFile*);
	virtual ~TaskRunner(void);

	//제어 인터페이스
	bool enterConstruct			(ExeTaskTreeNode*);
	void enterDestruct			(ExeTaskTreeNode*);
	void enterTask				(ExeTask*, RFSMtask*);
	//void enterTaskLoop			(ExeTask*, RFSMtask*);
	void enterBehavior			(ExeBehavior*, ValueMap);
	
	void enterConnector		    (ExeConnector*, ValueMap);
	void enterState				(ExeBehavior*, RFSMbehavior*, RFSMstate*);
	void link_Expand			(ExeTaskTreeNode* curNode, std::vector<ExeTransition*>);	
	//void runTaskTreeNode2		(ExeTaskTreeNode* curNode, ExeTransition* trans);
	void runDestructorInExeNode	(ExeTaskTreeNode* node);
	void expand_Connector			(ExeTaskTreeNode*, ExeTransition*);
	void expand_Behavior			(ExeTaskTreeNode*, ExeTransition*);
	void removeNotValidSubs		(ExeTaskTreeNode*, std::vector<ExeTransition*>);
	void enterChildNodeDestruct	(ExeTaskTreeNode*);
	void monitorSubOfRun		(ExeTaskTreeNode* cur, ExeTaskTreeNode* sub);
	void updateConnectorStatus	(ExeTaskTreeNode* cur, ExeTaskTreeNode* sub);

	void enterIfStmt		(RFSMifStmt*, ExeTaskTreeNode*);
	void enterDecisionBlock	(std::vector<RFSMstmt*>*, ExeTaskTreeNode*, std::vector<ExeTransition*>& translist, bool isPar);
	bool enterActionBlock	(std::vector<RFSMstmt*>*, ExeTaskTreeNode*);
	void initActionBlock	(std::vector<RFSMstmt*>*);
	void enterWithBlock4Par	(std::vector<RFSMstmt*>*, ExeTaskTreeNode*);

	//값을 실제로 evaluation
	ValueMap*		evalParams		(std::vector<RFSMparam*>*, ExeTaskTreeNode*, int line);
	bool			evalCondition	(RFSMboolable*,	ExeTaskTreeNode*, int line);
	RFSMvalueEval	evalValue		(RFSMvalue*, ExeTaskTreeNode*, int line);
	RFSMvalueEval	evalBinaryOp	(RFSMbinaryOp*,	ExeTaskTreeNode*, int line);

	/*binding*/
	//function action
	RFSMvalueEval bind4Function(ExeTaskTreeNode*, unsigned int callType, RFSMcallFunc* rfunc, ValueMap* params);
	//symbol
	RFSMvalueEval bind4GetModelSym(ExeTaskTreeNode*, RFSMvariable* sym, int line);
	void bind4SetModelSym(ExeTaskTreeNode*, RFSMvariable* lvar, RFSMvalueEval val, int line);
	//gvar
	RFSMvalueEval bind4GetGlobalVar(ExeTaskTreeNode*, RFSMvariable* sym, int line);
	void bind4SetGlobalVar(ExeTaskTreeNode*, RFSMvariable* lvar, RFSMvalueEval val, int line);
	//var
	RFSMvalueEval bind4GetTaskVar(ExeTaskTreeNode*, RFSMvariable* var, int line);
	void bind4SetTaskVar(ExeTaskTreeNode*, RFSMvariable* lvar, RFSMvalueEval val, int line);	
	//local
	RFSMvalueEval bind4GetLocalVar(ExeTaskTreeNode*, RFSMvariable* var, int line);
	void bind4SetLocalVar(ExeTaskTreeNode*, RFSMvariable* lvar, RFSMvalueEval val, int line);
	
	bool onBreak(ExeTaskTreeNode*, RFSMBlockInfo*, std::string);
	bool isBreakline(ExeTaskTreeNode*, std::string, int);
	void waitAtBreakline(ExeTaskTreeNode*);
	void monitorVariable(std::string file, int line, std::string, int, unsigned int, std::string, RFSMvalueEval val);
	void monitorFunction(std::string file, int line, std::string, int, std::string, ValueMap*);
	RFSMvalueEval getEmulvalue(std::string targetname, int type, int lint, std::string bname);
	//int dbgVariables(ExeTaskTreeNode* node, RFSMvariable* var, int line);

	/*디버깅*/
	void	setAsDebugMode		(void);
	bool	isDebugMode			(void);
	bool	isMonitorMode		(void);
	void	setAsMonitorMode	(void);
	int		setMove2NextBP_On	(int);
	int		setMove2NextStep_On	(int);
	int		addCondMu4BP		(int);
	int		delCondMu4BP		(int);
	void	wakeupAllBPwait		(void);
	int		setSuspendMonitor	(void);
	int		setResumeMonitor	(void);

	pthread_mutex_t*	getBPMutex	(int);
	pthread_cond_t*		getBPCond	(int);
};

const int NORMAL	= 1;
const int DEBUG		= 2;
const int MONITOR	= 3;