/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/

#include "runtime/RTObjBuilder.h"

RTObjBuilder::RTObjBuilder(std::ostream* ostr, SymTAB4Task* symT)
{
	m_symT = symT;
	m_ostr = ostr;
}

RTObjBuilder:: ~RTObjBuilder(void)
{
	//delete m_symT;
	//여기서 지우면 안된다.. 여기서는 생성만 해서 전체를 관리하는데로 넘겨주는 역할
}

void RTObjBuilder::printWarningMsg(int exceptionType, Gtoken* token)
{
	std::string msg = "";
	std::string msg0 = token->getData().c_str();
	msg0.append(", ");

	switch(exceptionType){
	
	case NO_BEHAVIOR_TO_PERFORM:
		msg = "There is no behavior in the loaded file(s).";break;
	
	default:
		break;

	}

	if(m_ostr != NULL){
		(*m_ostr)<<token->getFileName().c_str()<<", Line "<<token->getEndLine()<<": ";
		(*m_ostr)<<msg0.c_str()<<msg.c_str()<<"\r\n"<<"\t: Warning ..."<<"\r\n";
	}
	else{
		printf("%s, Line %d: ", token->getFileName().c_str(), token->getEndLine());
		printf("%s%s\n", msg0.c_str(), msg.c_str());
		printf("\t: %s\n", "Warning ...");
	}
}

void RTObjBuilder::printErrMsg(int exceptionType, Gtoken* token)
{
	std::string msg = "";
	std::string msg0 = token->getData().c_str();
	msg0.append(", ");

	switch(exceptionType){

	case NO_TASK_TO_PERFORM:
		msg = "There is no task to perform.";break;
	case TASK_NAME_DUPLICATED:
		msg = "The task name is duplicated.";break;
	case UNDEFINED_TYPE:
		msg = "Unknown type is used.";break;
	case SYM_ENUM_NOT_DEFIENED:
		msg = "Undefined enum type is used.";break;
	//이 부분을 따로 빼서 main에서 한꺼번에 처리하도록 바꾸자..
	case SYM_MODEL_CANNOT_BE_TYPE:
		msg = "The model can not be used as type.";break;

	case SYM_SYMBOL_ENUM_LOCALVAR_NOT_FOUND:
		msg = "The symbol/enum/variable is not defined.";break;
	case SYM_SYMBOL_NOT_FOUND:
		msg = "The symbol is not defined.";break;
	case SYM_ENUM_NOT_FOUND:
		msg = "The enum type is not defined.";break;
	case SYM_ACTION_NOT_FOUND:
		msg = "The action is not defined.";break;
	case SYM_FUNC_NOT_FOUND:
		msg = "The function is not defined.";break;
	case SYM_TASK_NOT_FOUND:
		msg = "The behavior is not defined.";break;
	case SYM_CONN_NOT_FOUND:
		msg = "The connector is not defined.";break;
	case SYM_BEHA_N_CONN_NOT_FOUND:
		msg = "The behavior/connetor is not defined";break;
	case SYM_STATE_NOT_FOUND:
		msg = "The state is not defined.";break;
	case SYM_LOCAL_VAR_NOT_FOUND:
		msg = "The variable is not defined";break;

	case LOCALVAR_NAME_DUPLICATED:
		msg = "The same name exists among the name of enums/models/states or other local variales";break;
	case SYM_SYMBOL_DUPLICATED:
		msg = "The symbol name is duplicated";break;
	case SYM_MODEL_DUPLICATED:
		msg = "The model name is duplicated";break;
	case SYM_ACTION_DUPLICATED:
		msg = "The action name is duplicated.";break;
	case SYM_FUNC_DUPLICATED:
		msg = "The function name is duplicated.";break;
	case SYM_ENUM_DUPLICATED:
		msg = "The enum name is duplicated.";break;
	case SYM_STATE_DUPLICATED:
		msg = "The state name is duplicated.";break;
	case SYM_TASK_DUPLICATED:
		msg = "The behavior name is duplicated.";break;
	case SYM_CONN_DUPLICATED:
		msg = "The connector name is duplicated.";break;
	case SYM_LOCAL_VAR_DUPLICATED:
		msg = "The variable name is duplicated.";break;
	case SYM_SYSVAR_DUPLICATED:
		msg = "The system variable is duplicated.";break;

	case WRONG_OP_ERROR:
		msg = "The operator ~> cannot be used in a connector.";break;

	case SYM_TYPE_MISS_MATCHED:
		msg = "The data type is irrelevant for data operation.";break;
	case SYM_LVALUE_MUSTBE_VAR:
		msg = "The left of assignment operator should be a variable";break;


	case TYP_UNCOMPATIBLE:
		msg = "Operand type is not compatible.";break;
	case TYP_RVALUE_IS_BIGGER:
		msg = "The variable type is not casted implicitly";break;
	case SYM_FUNC_PARAM_NAME_WRONG:
		msg = "The parameter's name is wrong.";break;
	case SYM_FUNC_PARAM_TYPE_WRONG:
		msg = "The parameter's type is not identical.";break;
	case SYM_FUNC_PARAM_FORM_WRONG:
		msg = "Follow the form, parameter's name = paramter's value.";break;

	case CONNECTOR_TYPE_JOIN_MISMATCH:
		msg = "A connector conexer must has its join type (and-join and or-join).";break;

	case ACTION_IN_TRAN_BL_ERROR:
		msg = "In transition block, action cannot be called.";break;
	case TASK_IN_ACTION_BL_ERROR:
		msg = "Sub-behavior cannot be called in Entry/Do/Exit action blocks.";break;
	case GOTO_IN_ACTION_BL_ERROR:
		msg0="";
		msg = "Goto statement cannot be used in Entry/Do/Exit action blocks.";break;
	case NOT_BOOL_EXPR:
		msg = "If-condition should be logical expression.";break;

	case WRONG_SYMBOL_USED_IN_EXPAND_STMT:
		msg = "In a run block, only the <break> symbol can be used as a post action of an <expand> statement.";break;
	case WRONG_STMT_IN_ACTION_BLOCK:
		msg = "the statement cannot be used in action blocks.";break;
	case WRONG_STMT_USED_IN_DECI_BLOCK:
		msg = "the statement cannot be used in decision blocks.";break;
	case NON_DETERMINISTIC_LINK_ERROR:
		msg = "A Transition/Run block violate deterministic link to a sun-node.";break;
	case UNREACHABLE_STMT_ERROR:
		msg = "The statement after a <expand> statement is unreachable.";break;
	default:
		break;

	}

	if(m_ostr != NULL){
		(*m_ostr)<<token->getFileName().c_str()<<", Line "<<token->getEndLine()<<": ";
		(*m_ostr)<<msg0.c_str()<<msg.c_str()<<"\r\n"<<"\t: syntax error..."<<"\r\n";
	}
	else{
		printf("%s, Line %d: ", token->getFileName().c_str(), token->getEndLine());
		printf("%s%s\n", msg0.c_str(), msg.c_str());
		printf("\t: %s\n", "syntax or semantics error...");
	}
}

void RTObjBuilder::makeSymTable(GparsedResult* pRslt) throw (ErrorHandler&)
{
	EList<Genum*>*		enumlist	= pRslt->getEnums();
	EList<Gmodel*>*		modellist	= pRslt->getModels();
	EList<Gaction*>*	actionlist	= pRslt->getActions();
	//EList<Gworker*>*	workerlist	= pRslt->getWorkers();
	//EList<Gtask*>*	tasklist	= pRslt->getTasks();
	EList<Gbehavior*>*	bhvlist		= pRslt->getbehaviors();
	EList<Gconnector*>* conlist		= pRslt->getconnectors();

	SemanticsEH eHandler;

	//아래 심벌테이블을 만드는 순서도 중요하다.
	makeSymT(eHandler, enumlist, "");
#ifdef __DEBUG_SYMBOL_
	m_symT->printEnums();
#endif

	makeSymT(eHandler, modellist,"");
#ifdef __DEBUG_SYMBOL_
	m_symT->printModels();
	m_symT->printFunctions();
#endif

	makeSymT(eHandler, actionlist,"");
#ifdef __DEBUG_SYMBOL_
	m_symT->printActions();
#endif

	makeSymT(eHandler, bhvlist,"");
#ifdef __DEBUG_SYMBOL_
	m_symT->printStates();
#endif

	makeSymT(eHandler, conlist,"");

	if(eHandler.hasError_() != 0)
		throw eHandler;
}

/***************************************************************************

선언된 MODEL, ENUM, ACTION에 대해서 검사하고 SYMBOL 테이블을 생성한다.

****************************************************************************/

void RTObjBuilder::makeSymT(ErrorHandler& eHandler, EList<Gmodel*>* modellist, std::string parent)
{
	if(modellist != 0 && modellist->getSize()>0){
		if(modellist->moveHead()){
			do{
				Gmodel* model = modellist->getCurObject();

				std::string prefix = parent;
				prefix.append(model->getName()->getData().c_str());

				int result = m_symT->addModel(prefix); // ball, ball.seen, ball.position
				if(result != 0){
					eHandler.addErrorCount();
					printErrMsg(result, model->getName());
				}

				prefix.append(".");

				/* variable */
				EList<GmodelVar*>* varlist = model->getVariables();
				if(varlist != 0 && varlist->getSize()>0 && varlist->moveHead()){
					do{
						GmodelVar* gVar = varlist->getCurObject();
						checkVariableType(eHandler, gVar->getType());

						std::string vName = prefix;
						vName = vName.append(gVar->getName()->getData().c_str());
						unsigned int mode = gVar->getMode();
						unsigned int varType = getType(gVar->getType()->getData());
						if(varType == TYP_NOT_PRIMITIVE)
							varType = m_symT->getUserTypeID(gVar->getType()->getData());
						TABsymbol *sym = new TABsymbol(prefix, "model", vName, varType, mode, "");

						//중복검사
						int result = m_symT->addSymbol(sym);
						if(result != 0){
							eHandler.addErrorCount();
							printErrMsg(result, model->getName());
						}
					}
					while(varlist->moveNext());
				}

				/* function */
				EList<Gfunction*>* funclist = model->getFunctions();
				if(funclist != 0 && funclist->getSize()>0 && funclist->moveHead()){
					do{
						Gfunction *func = funclist->getCurObject();
						std::string fName = prefix;
						fName.append(func->getName()->getData().c_str());
						//aName.append(".");

						//파라메터 검사
						EList<Gparameter*>* pList = func->getParameters();
						checkFuncParamsDecl(eHandler, pList);

						//새로운 TABfunction 생성
						unsigned int rtnType = getType(func->getReturnType()->getData());
						if(rtnType == TYP_NOT_PRIMITIVE){
							rtnType = m_symT->getUserTypeID(func->getReturnType()->getData());
							checkVariableType(eHandler, func->getReturnType());
						}
						TABfunction *rFunc = new TABfunction(rtnType, fName, NULL);
						EList<TABparameter*>* tlist = makeTABparams(pList);
						rFunc->setParameters(tlist);
						if(func->isAction()) rFunc->setAsAction();
						/*
						if(pList != 0 &&  pList->getSize()>0 &&pList->moveHead()){
							do{
								Gparameter* gPar = pList->getCurObject();
								TABparameter* tPar = new TABparameter(gPar->getType()->getData(), gPar->getName()->getData());
								rFunc->addParameter(tPar);
							}
							while(pList->moveNext());
						}
						*/
						//중복검사
						int result = m_symT->addFunction(rFunc);
						if(result != 0){
							eHandler.addErrorCount();
							printErrMsg(result, func->getName());
						}
					}
					while(funclist->moveNext());
				}

				/* model  모델내부에 모델이 정의된 경우*/
				EList<Gmodel*>* modelList2 = model->getModels();
				makeSymT(eHandler, modelList2, prefix);
			}
			while(modellist->moveNext());
		}
	}


	//m_symT->makeSymTAB(modellist, parent);
}

void RTObjBuilder::makeSymT(ErrorHandler& eHandler, EList<Genum*>* enumlist, std::string prefix)
{
	if(enumlist != 0 && enumlist->getSize()>0){
		if(enumlist->moveHead()){
			do{
				Genum *e = enumlist->getCurObject();
				Gtoken* eTypeName = e->getName();
				std::string eName = prefix;
				eName.append(eTypeName->getData().c_str());
				eName.append(".");
				EList<Gtoken*>* elems = e->getElements();
				unsigned int idx = 1;
				if(elems != 0 && elems->getSize()>0 && elems->moveHead()){
					do{
						std::string elem = eName;
						Gtoken* elemName = elems->getCurObject();
						elem.append(elemName->getData().c_str());
						TABenum *rEnum = new TABenum(eTypeName->getData().c_str(), elem, idx++);

						//중복검사
						int result  = m_symT->addEnum(rEnum);
						if(result != 0){
							eHandler.addErrorCount();
							printErrMsg(result, elemName);
						}
					}
					while(elems->moveNext());
					//Enum 내부에 enum이 정의 되는 경우도 있을 듯 나중에 고려 ->문법도 교체
				}
			}
			while(enumlist->moveNext());
		}
	}
}

void RTObjBuilder::makeSymT(ErrorHandler& eHandler, EList<Gaction*>* actionlist, std::string prefix)
{
	if(actionlist != 0 && actionlist->getSize()>0){
		if(actionlist->moveHead()){
			do{
				Gaction * action = actionlist->getCurObject();
				std::string aName = prefix;
				aName.append(action->getName()->getData().c_str());
				//aName.append(".");

				//파라메터 검사
				EList<Gparameter*>* pList = action->getParameters();
				checkFuncParamsDecl(eHandler, pList);

				//TABaction을 생성
				TABaction *ract = new TABaction(aName, NULL);
				EList<TABparameter*>* tlist = makeTABparams(pList);
				ract->setParameters(tlist);

				//중복검사
				int result = m_symT->addAction(ract);
				if(result != 0){
					eHandler.addErrorCount();
					printErrMsg(result, action->getName());
				}
			}
			while(actionlist->moveNext());
		}
	}
}

EList<TABparameter*>* RTObjBuilder::makeTABparams(EList<Gparameter*>* pList)
{
	EList<TABparameter*>* tlist = 0;
	if(pList != 0 &&  pList->getSize()>0 && pList->moveHead()){
		tlist = new EList<TABparameter*>();
		do{
			Gparameter* gPar = pList->getCurObject();
			TABparameter* tPar = new TABparameter(gPar->getType()->getData(), gPar->getName()->getData());
			tlist->addTail(tPar);
		}
		while(pList->moveNext());
	}
	return tlist;
}

void RTObjBuilder::makeSymT(ErrorHandler& eHandler, EList<Gbehavior*>* bhvlist, std::string prefix)
{
	if(bhvlist != 0 && bhvlist->getSize()>0){
		if(bhvlist->moveHead()){
			do{
				Gbehavior *bhv = bhvlist->getCurObject();

				std::string t = prefix;	//지금은 사용안되지만 나중에 namespace를 위해서
				Gtoken *tName = bhv->getName();
				t.append(tName->getData().c_str());

				//파라메터 검사
				EList<Gparameter*>* pList = bhv->getParameters();
				checkFuncParamsDecl(eHandler, pList);

				//TABtask를 생성
				TABbehavior *rtask = new TABbehavior(t, NULL);
				EList<TABparameter*>* tlist = makeTABparams(pList);
				rtask->setParameters(tlist);

				//중복검사
				int result = m_symT->addBehavior(rtask);
				if(result != 0){
					eHandler.addErrorCount();
					printErrMsg(result, bhv->getName());
				}


				/* state */
				EList<Gstate*>* statelist = bhv->getStates();
				if(statelist != 0 && statelist->getSize()>0 && statelist->moveHead()){
					do{
						Gstate *state = statelist->getCurObject();
						Gtoken *sName = state->getName();

						//중복검사
						TABstate *rstate = new TABstate(t, sName->getData());
						int result = m_symT->addState(rstate);
						if(result != 0){
							eHandler.addErrorCount();
							printErrMsg(result, state->getName());
						}
					}
					while(statelist->moveNext());
				}
			}
			while(bhvlist->moveNext());
		}
	}
}

void RTObjBuilder::makeSymT(ErrorHandler& eHandler, EList<Gconnector*>* conlist, std::string prefix)
{
	if(conlist != 0 && conlist->getSize()>0){
		if(conlist->moveHead()){
			do{
				Gconnector *con = conlist->getCurObject();

				std::string t = prefix;	//지금은 사용안되지만 나중에 namespace를 위해서
				Gtoken *cName = con->getName();
				t.append(cName->getData().c_str());

				//파레메터 검사
				EList<Gparameter*>* pList = con->getParameters();
				checkFuncParamsDecl(eHandler, pList);

				//TABconnector를 생성
				TABconnector *pCon = new TABconnector(t, NULL);
				EList<TABparameter*>* tlist = makeTABparams(pList);
				pCon->setParameters(tlist);

				//중복검사
				int result = m_symT->addConnector(pCon);
				if(result != 0){
					eHandler.addErrorCount();
					printErrMsg(result, con->getName());
				}
			}
			while(conlist->moveNext());
		}
	}
}

/*
void RFSMsemantics::checkDuplication(ErrorHandler& eHandler, std::string name)
{
	//
}
*/

void RTObjBuilder::checkFuncParamsDecl(ErrorHandler& eHandler, EList<Gparameter*>* pars)
{
	if(pars != 0 && pars->getSize()>0 && pars->moveHead()){
		Gparameter *param = pars->getCurObject();
		checkFuncParameter(eHandler, param);
	}
}

/**
* model내에 정의된 함수나 action들의 파라메터를 검사
*/
void RTObjBuilder::checkFuncParameter(ErrorHandler& eHandler, Gparameter* param)
{
	Gtoken* type = param->getType();
	Gtoken* name = param->getName();

	checkVariableType(eHandler, type);
}

/**
* 모델내의 변수선언에 대해서 검사
*/
void RTObjBuilder::checkVariableType(ErrorHandler& eHandler, Gtoken* type)
{
	unsigned int varType = getType(type->getData().c_str());
	if(varType == TYP_NOT_PRIMITIVE){
		//enum에 해당 타입이 있는지 검사
		int result = m_symT->findEnumType(type->getData().c_str());
		if(result != 0){
			eHandler.addErrorCount();
			//printErrMsg(result, type);
			printErrMsg(UNDEFINED_TYPE, type);
		}
		//혹시 model을 타입으로 사용했는지 검사
		result = m_symT->findModel(type->getData().c_str());
		if(result == 0){//model이름이 타입으로 사용된 경우
			eHandler.addErrorCount();
			printErrMsg(SYM_MODEL_CANNOT_BE_TYPE, type);
		}
	}
}
/****************************************************************************************************

								  TASK에 대해서 검사

*****************************************************************************************************/
EList<RFSMtask*>* RTObjBuilder::checkSemantics(EList<Gtask*>* tasklist) throw(ErrorHandler&)
{
	SemanticsEH eHandler;

	EList<RFSMtask*>* rtasks = NULL;
	if(tasklist != 0 && tasklist->getSize()>0 && tasklist->moveHead()){
		rtasks = new EList<RFSMtask*>();
		do{
			Gtask* task = tasklist->getCurObject();
			Gtoken* name = task->getName();

#ifdef __DEBUG_SYMBOL_
std::cout<<"TASK "<<name->getData().c_str()<<"\n";
#endif
			RFSMtask* rtask = checktask(eHandler, task);

			//중복검사
			if(rtasks !=NULL && rtasks->getSize()>0 && rtasks->moveHead()){
				do{
					RFSMtask* rtask0 = rtasks->getCurObject();
					if(rtask0->getName() == rtask->getName()){//같은 이름이 존재
						eHandler.addErrorCount();
						printErrMsg(TASK_NAME_DUPLICATED, task->getName());
						throw eHandler;
					}
				}
				while(rtasks->moveNext());
			}
			//중복이 없으면 추가
			rtasks->addTail(rtask);
		}
		while(tasklist->moveNext());

		if(eHandler.hasError_() > 0)
		throw eHandler;
	}
	else{
		eHandler.addErrorCount();
		Gtoken* tok = new Gtoken();
		printErrMsg(NO_TASK_TO_PERFORM, tok);
		delete tok;
		throw eHandler;
	}

#ifdef __DEBUG_SYMBOL_
	m_symT->printLocalVariables();
#endif

	return rtasks;
}

RFSMtask* RTObjBuilder::checktask(ErrorHandler& eHandler, Gtask* task)
{
	if(task == 0) return 0;

	Gtoken* tname = task->getName();
	RFSMtask* rtask = new RFSMtask(tname->getData(), tname->getEndLine(), task->getEndLine(), task->getBlockID(), task->getFileName());
	m_curTreeNode = rtask;

	GstmtBlock* cons = task->getConstruct();
	GstmtBlock* dest = task->getDestruct();
	GstmtBlock* runb = task->getRunBlock();

	//task 변수(이걸 가장 먼저 해야함 순서 중요)
	std::string tid = task->getBlockID();
	EList<Gstmt*>* taskVars = task->getCstVars();
	checkVarsBlock(eHandler, taskVars, rtask);
/*	if(taskVars != NULL && taskVars->getSize()>0){
		do{
			GlocalVar* var = (GlocalVar*)(taskVars->getCurObject());
			RFSMasmt* rstmt = checkLocalVarDecl(eHandler, var, tid);
			rstmt->setBlockFullName(var->getBlockFullName());
			rstmt->setEOL(var->getEndLine());
			if(rstmt != NULL)
				rtask->addVarAsmt(rstmt);
		}while(taskVars->moveNext());			
	}
*/

	if(cons != NULL){
		EList<Gstmt*> cb;
		cb.addTail(cons);
		std::vector<RFSMstmt*>* rcons = checkActionStmts(eHandler, &cb);
		//std::vector<RFSMstmt*>* rcons = checkActionStmts(eHandler, cons->getStmts());
		rtask->setConstruct(rcons);
		rtask->setEOLconst(cons->getEndLine());
		cb.delTail();
	}

	if(dest != NULL){
		EList<Gstmt*> db;
		db.addTail(dest);
		std::vector<RFSMstmt*>* rdest = checkActionStmts(eHandler, &db);
		//std::vector<RFSMstmt*>* rdest = checkActionStmts(eHandler, dest->getStmts());
		rtask->setDestruct(rdest);
		rtask->setEOLdest(dest->getEndLine());
		db.delTail();
	}

	if(runb != NULL){
		EList<Gstmt*> rb;
		rb.addTail(runb);
		std::vector<RFSMstmt*>* rrunb = checkTransStmts(eHandler, &rb, true);
		int num = 0;
		checkDeterminism(eHandler, runb, num);
		//std::vector<RFSMstmt*>* rrunb = checkTransStmts(eHandler, runb->getStmts());
		rtask->setRunBlock(rrunb);
		rtask->setEOLrun(runb->getEndLine());
		rb.delTail();
	}

	return rtask;
}

/****************************************************************************************************

								  BEHAVIOR에 대해서 검사

*****************************************************************************************************/

EList<RFSMbehavior*>* RTObjBuilder::checkSemantics(EList<Gbehavior*>* bhvlist) throw(ErrorHandler&)
{
	SemanticsEH eHandler;

	EList<RFSMbehavior*>* rbhvs = NULL;
	if(bhvlist != 0 && bhvlist->getSize()>0 && bhvlist->moveHead()){
		rbhvs = new EList<RFSMbehavior*>();
		do{
			Gbehavior* bhv = bhvlist->getCurObject();
			Gtoken* name = bhv->getName();

#ifdef __DEBUG_SYMBOL_
std::cout<<"TASK "<<name->getData().c_str()<<"\n";
#endif
			RFSMbehavior* rbhv = checkBehavior(eHandler, bhv);

			rbhvs->addTail(rbhv);
		}
		while(bhvlist->moveNext());

		if(eHandler.hasError_() > 0){
			//만들다 만것들을 지우고
			if(rbhvs != NULL){
				while(rbhvs->getSize()>0 && rbhvs->moveHead()){
					RFSMbehavior* obj = rbhvs->getCurObject();
					delete obj;
					rbhvs->delHead();
				}
				delete rbhvs;		
			}			
			//에러 throw
			throw eHandler;
		}
	}
	else{
		//eHandler.addErrorCount();
		Gtoken tok = Gtoken();
		printWarningMsg(NO_BEHAVIOR_TO_PERFORM, &tok);
		//throw eHandler;
	}

#ifdef __DEBUG_SYMBOL_
	m_symT->printLocalVariables();
#endif

	return rbhvs;
}

EList<RFSMconnector*>* RTObjBuilder::checkSemantics(EList<Gconnector*>* connlist) throw(ErrorHandler&)
{
	SemanticsEH eHandler;

	EList<RFSMconnector*>* rconnlist = NULL;
	if(connlist != 0 && connlist->getSize()>0 && connlist->moveHead()){
		rconnlist = new EList<RFSMconnector*>();
		do{
			Gconnector* gconn = connlist->getCurObject();
			RFSMconnector* rconn = checkConnector(eHandler,  gconn);
			rconnlist->addTail(rconn);
		}
		while(connlist->moveNext());

		if(eHandler.hasError_() > 0)
		throw eHandler;
	}
	return rconnlist;
}

void RTObjBuilder::checkVarsBlock(ErrorHandler& eHandler, EList<Gstmt*>* varList, RFSMcst* node)
{	
	if(varList != NULL && varList->getSize()>0){
		do{
			GlocalVar* var = (GlocalVar*)(varList->getCurObject());
			RFSMstmt* rstmt = checkLocalVarDecl(eHandler, var);			
			if(rstmt != NULL){
				rstmt->setBlockID(var->getBlockPath());
				rstmt->setEndLine(var->getEndLine());
				node->addVarAsmt(rstmt);
			}

			//
		}while(varList->moveNext());			
	}
}

void RTObjBuilder::checkBhvConnParams(ErrorHandler& eHandler, EList<Gparameter*>* params)
{
	if(params!= NULL && params->getSize()>0 && params->moveHead()){
		do{
			Gparameter* param = params->getCurObject();
			Gtoken* ntok = param->getName();
			Gtoken* ttok = param->getType();
			//int result = m_symT->findLocalVar(ntok->getBlockID(), ntok->getData()); //이런경우는 없다. bhv시작지점에 우선적으로오기 때문에
			int result = m_symT->checkMoldelDuplication(ntok->getData());
			if(result == SYM_NOT_DUPLICATED){//없으면
				TABsymbol* sym = new TABsymbol(ntok->getBlockPath(), ntok->getBlockName(), ntok->getData(), getType(ttok->getData()),1000,"");
				int r2 = m_symT->addLocalVar(ntok->getBlockPath(), sym);
				if(r2==SYM_LOCAL_VAR_DUPLICATED){
					eHandler.addErrorCount();
					printErrMsg(LOCALVAR_NAME_DUPLICATED, ntok);
				}
			}
			else{
				eHandler.addErrorCount();
				//error메시시: 중복정의 되었습니다.
				//printErrMsg(result, ntok);
				printErrMsg(LOCALVAR_NAME_DUPLICATED, ntok);
			}
		}
		while(params->moveNext());
	}
}

RFSMbehavior* RTObjBuilder::checkBehavior(ErrorHandler& eHandler, Gbehavior* bhv)
{
	if(bhv == 0) return 0;

	Gtoken* bname = bhv->getName();
	RFSMbehavior* rbhv = new RFSMbehavior(bname->getData(), bname->getEndLine(), bhv->getEndLine(), bhv->getBlockID(), bhv->getFileName(), bhv->isMission());
	m_curTreeNode = rbhv;

	//parameter->symbol테이블 생성할 때는 TASK자체에 대한 정보가 저장
	//파라메터가 TASK block의 변수로써 참조될 수 있는 용도 아님

	std::string tid = bhv->getBlockID();
	EList<Gparameter*>* params = bhv->getParameters();
	checkBhvConnParams(eHandler, params);

	GstmtBlock* cons = bhv->getConstruct();
	GstmtBlock* dest = bhv->getDestruct();

	//behvior 변수(이걸 가장 먼저해야함 순서 중요)
	EList<Gstmt*>* bhvVars = bhv->getCstVars();
	checkVarsBlock(eHandler, bhvVars, rbhv);
	/*if(bhvVars != NULL && bhvVars->getSize()>0){
		do{
			GlocalVar* var = (GlocalVar*)(bhvVars->getCurObject());
			RFSMasmt* rstmt = checkLocalVarDecl(eHandler, var, tid);
			rstmt->setBlockFullName(var->getBlockFullName());
			rstmt->setEOL(var->getEndLine());
			if(rstmt != NULL)
				rbhv->addVarAsmt(rstmt);
		}while(bhvVars->moveNext());			
	}*/

	if(cons != NULL){
		//디버깅시에 construct의 }로 정지점이 이동되게 하기위함, 코드를 다 고치기 힘들어서 그냥 편법
		EList<Gstmt*> cb;
		cb.addTail(cons);
		std::vector<RFSMstmt*>* rcons = checkActionStmts(eHandler, &cb);
		//std::vector<RFSMstmt*>* rcons = checkActionStmts(eHandler, cons->getStmts());
		cb.delTail();
		rbhv->setConstruct(rcons);
		rbhv->setEOLconst(cons->getEndLine());
	}

	if(dest != NULL){
		//디버깅시에 destruct의 }로 정지점이 이동되게 하기위함, 코드를 다 고치기 힘들어서 그냥 편법
		EList<Gstmt*> db;
		db.addTail(dest);
		std::vector<RFSMstmt*>* rdest = checkActionStmts(eHandler, &db);
		//std::vector<RFSMstmt*>* rdest = checkActionStmts(eHandler, dest->getStmts());
		db.delTail();
		rbhv->setDestruct(rdest);
		rbhv->setEOLdest(dest->getEndLine());
	}

	//state
	EList<Gstate*>* statelist = bhv->getStates();
	//EList<RFSMstate*>* rstates = NULL;
	std::vector<RFSMstate*>* rstates = NULL;
	if(statelist != 0 && statelist->getSize()>0 && statelist->moveHead()){
		rstates = new std::vector<RFSMstate*>();
		do{
			Gstate* state = statelist->getCurObject();
			std::string sid = state->getBlockID();
			Gtoken* sn = state->getName();
			RFSMstate* rstate = new RFSMstate(state->getBlockName(), sn->getData(), sn->getEndLine(), sid, state->getFileName(), state->getEndLine());
			rstate->setModifier(state->getModifier());


			//현재 체크 중인 RFSMstate
			m_curState = rstate;

			//state 변수(이걸 가장 먼저해야 한다. 위치 중요)
			EList<Gstmt*>* stateVars = state->getStateVars();
			checkVarsBlock(eHandler, stateVars, rstate);
			
/*
			std::string sp = tid;
			sp.append(".").append(sid);
			if(stateVars != NULL && stateVars->getSize()>0){				
				do{
					GlocalVar* var = (GlocalVar*)(stateVars->getCurObject());
					RFSMasmt* rstmt = checkLocalVarDecl(eHandler, var, sp);
					rstmt->setBlockFullName(var->getBlockFullName());
					rstmt->setEOL(var->getEndLine());
					if(rstmt != NULL)
						rstate->addVarAsmt(rstmt);
				}while(stateVars->moveNext());			
			}
*/
			//transition
			GstateTrans* trans = state->getTransition();		
			if(trans!= NULL){
				rstate->setEOLtrans(trans->getEndLine());
				std::string trid = trans->getBlockID();
				std::string tpath = tid;
				tpath.append(".").append(sid).append(".").append(trid);
				//RFSMtransition* rtrans = checkTransition(eHandler, trans, tpath);
				EList<Gstmt*>* transBlock = trans->getStmts();
				//EList<RFSMstmt*>* rtrans = checkTransition(eHandler, transBlock, tpath);
				std::vector<RFSMstmt*>* rtrans = checkTransStmts(eHandler, transBlock, false);
				int num = 0;
				checkDeterminism(eHandler, trans, num);
				rstate->setTransition(rtrans);
			}			

			//entry, do, exit
			GstateDo* gDo = state->getDoBlock();		
			if(gDo!= NULL){
				rstate->setEOLstay(gDo->getEndLine());
				std::string doid = gDo->getBlockID();
				std::string dopath = tid;
				dopath.append(".").append(sid).append(".").append(doid);
				EList<Gstmt*>* doBlock = gDo->getStmts();
				//디버깅시에 stay의 }로 정지점이 이동되게 하기위함, 코드를 다 고치기 힘들어서 그냥 편법
				EList<Gstmt*> db;
				db.addTail(gDo);
				std::vector<RFSMstmt*>* dostmts = checkActionStmts(eHandler, &db);
				db.delTail();
				//std::vector<RFSMstmt*>* dostmts = checkActionStmts(eHandler, doBlock);
				
				rstate->setDostmts(dostmts);
			}

			GstateEntry* gEntry =state->getEntryBlock();			
			if(gEntry!= NULL){
				rstate->setEOLconst(gEntry->getEndLine());
				std::string entryid = gEntry->getBlockID();
				std::string entrypath = tid;
				entrypath.append(".").append(sid).append(".").append(entryid);
				EList<Gstmt*>* enBlock = gEntry->getStmts();
				//디버깅시에 entry의 }로 정지점이 이동되게 하기위함, 코드를 다 고치기 힘들어서 그냥 편법
				EList<Gstmt*> eb;
				eb.addTail(gEntry);
				std::vector<RFSMstmt*>* enstmts = checkActionStmts(eHandler, &eb);
				//std::vector<RFSMstmt*>* enstmts = checkActionStmts(eHandler, enBlock);
				eb.delTail();
				rstate->setConstruct(enstmts);
			}

			GstateExit* gExit = state->getExitBlock();			
			if(gExit!= NULL){
				rstate->setEOLdest(gExit->getEndLine());
				std::string exitid = gExit->getBlockID();
				std::string exitpath = tid;
				exitpath.append(".").append(sid).append(".").append(exitid);
				EList<Gstmt*>* exBlock = gExit->getStmts();
				//디버깅시에 exit의 }로 정지점이 이동되게 하기위함, 코드를 다 고치기 힘들어서 그냥 편법
				EList<Gstmt*> exb;
				exb.addTail(gExit);
				std::vector<RFSMstmt*>* exstmts = checkActionStmts(eHandler, &exb);
				exb.delTail();
				//std::vector<RFSMstmt*>* exstmts = checkActionStmts(eHandler, exBlock);
				rstate->setDestruct(exstmts);
			}

			rstates->push_back(rstate);

		}
		while(statelist->moveNext());
	}

	rbhv->setStates(rstates);
//	rbhv->setSynchIDs(bhv->getSynchInfo());
	return rbhv;
}

RFSMconnector* RTObjBuilder::checkConnector(ErrorHandler& eHandler, Gconnector* conn)
{	
	Gtoken* cname = conn->getName();
	RFSMconnector* rconn = new RFSMconnector(conn->getConnType(), cname->getData(), cname->getEndLine(), conn->getEndLine(), conn->getBlockID(), conn->getFileName(), conn->isMission());		
	m_curTreeNode = rconn;
	rconn->setEndLine(conn->getEndLine());

	//std::string cid = conn->getBlockID();
	EList<Gparameter*>* params = conn->getParameters();
	checkBhvConnParams(eHandler, params);

	GstmtBlock* cnst = conn->getConstruct();
	GstmtBlock* dest = conn->getDestruct();

	//behvior 변수(이걸 가장 먼저해야함 순서 중요)
	EList<Gstmt*>* conVars = conn->getCstVars();
	checkVarsBlock(eHandler, conVars, rconn);

	if(cnst != NULL){
		//디버깅시에 construct의 }로 정지점이 이동되게 하기위함, 코드를 다 고치기 힘들어서 그냥 편법
		EList<Gstmt*> cb;
		cb.addTail(cnst);
		std::vector<RFSMstmt*>* rcons = checkActionStmts(eHandler, &cb);
		//std::vector<RFSMstmt*>* rcons = checkActionStmts(eHandler, cnst->getStmts());
		cb.delTail();
		rconn->setConstruct(rcons);
		rconn->setEOLconst(cnst->getEndLine());
	}

	if(dest != NULL){
		//디버깅시에 destruct의 }로 정지점이 이동되게 하기위함, 코드를 다 고치기 힘들어서 그냥 편법
		EList<Gstmt*> db;
		db.addTail(dest);
		std::vector<RFSMstmt*>* rdest = checkActionStmts(eHandler, &db);
		//std::vector<RFSMstmt*>* rdest = checkActionStmts(eHandler, dest->getStmts());
		db.delTail();
		rconn->setDestruct(rdest);
		rconn->setEOLdest(dest->getEndLine());
	}

	if(conn->getConnType() == CONEXER){
		int jtype = conn->getJoinType();
		if(jtype != 1 && jtype!=2){
			eHandler.addErrorCount();
			printErrMsg(CONNECTOR_TYPE_JOIN_MISMATCH, conn->getName());
		}
		else{
			rconn->setJoinType(conn->getJoinType());
		}
	}
	/*
	else if(conn->getConnType() == SEQEXER){
		int jtype = conn->getJoinType();
		if(jtype == 1 || jtype==2){
			eHandler.addErrorCount();
			printErrMsg(CONNECTOR_TYPE_JOIN_MISMATCH, conn->getConnectorName());
		}
	}
	*/
	
	rconn->setRunType(conn->getRunType());

	//conexer 삽입
	GstmtBlock* conexer = conn->getWiths();
	EList<Gstmt*> withs2;
	withs2.addTail(conexer);
	std::vector<RFSMstmt*>* rwiths = checkTransStmts(eHandler, &withs2, true);
	if(rwiths != NULL && rwiths->size()>0)
		rconn->setRunBlocks((RFSMStmtBlock*)rwiths->at(0));

	withs2.delTail();
	delete rwiths;//껍데기 삭제

	//Expand 문의 수 체크
	EList<Gstmt*>* stmtlist = conexer->getStmts();
	if(stmtlist !=NULL && stmtlist->moveHead()){			
		do{
			GstmtBlock *run = (GstmtBlock*)(stmtlist->getCurObject());
			int num = 0;
			checkDeterminism(eHandler, run, num);
		}
		while(stmtlist->moveNext());
	}
		
	/* SEQEXER지원 때문에 필요
	//withblock의 ID삽입
	EList<std::string>* withIDs = NULL;
	EList<Gstmt*>* stmtlist = conn->getStmts();
	if(stmtlist !=NULL && stmtlist->moveHead()){			
		do{
			//connector는 무조건 withBlock이기 때문에
			GstmtBlock *block = (GstmtBlock*)(stmtlist->getCurObject());
			if(block != NULL && withIDs == NULL)
				withIDs = new EList<std::string>();
			withIDs->addTail(block->getBlockID());			
		}
		while(stmtlist->moveNext());
	}
	rconn->setWithIDs(withIDs);
	*/

	return rconn;
}

/**
*Tranistion은 if문장만을 가진다.
*/
/*
RFSMtransition* RTObjBuilder::checkTransition(ErrorHandler& eHandler, GstateTrans* trans, const std::string blockpath)
{
	if(trans == 0) return 0;

	RFSMtransition* rtrans = NULL;
	
	EList<GstmtIf*>* iflist = trans->getIfstmts();
	if(iflist != 0 && iflist->getSize()>0 && iflist->moveHead()){
		rtrans = new RFSMtransition();
		
		EList<RFSMifStmt*>* riflist = new EList<RFSMifStmt*>();
		do{
			GstmtIf* ifstmt = iflist->getCurObject();
			RFSMifStmt* rifstmt = checkIfstmt(eHandler, ifstmt, true);
			if(rifstmt != NULL) riflist->addTail(rifstmt);
		}
		while(iflist->moveNext());
		rtrans->setIfstmts(riflist);
	}
	
	return rtrans;
}
*/

/*
EList<RFSMstmt*>* RTObjBuilder::checkTransition(ErrorHandler& eHandler, GstmtBlock *stmtBlock, const std::string blockpath)
{
	if(stmtBlock == 0) return 0;

	EList<RFSMstmt*>* stmtList = NULL;

	EList<Gstmt*>* stmts = stmtBlock->getStmts();
	if(stmts != 0 && stmts->moveHead()){

		stmtList = new EList<RFSMstmt*>();

		do{
			RFSMstmt* rstmt = NULL;
			Gstmt* stmt = stmts->getCurObject();
			//statement 종류 5가지
			if(stmt !=0){
				if(typeid(*stmt)==typeid(GstmtIf)){
					rstmt = checkIfstmt(eHandler, (GstmtIf*)stmt, true);
				}
				else if(typeid(*stmt)==typeid(GstmtCall)){
					//함수도 올 수 있고 Behavior도 올 수 있다..
					//만약 behavior가 오면 다음 state가 실행이 안된다.
				}
				else if(typeid(*stmt)==typeid(GstmtExpr)){
				}
				else if(typeid(*stmt)==typeid(GstmtGotoState)){
				}
				else if(typeid(*stmt)==typeid(GlocalVar)){
				}

				if(rstmt != NULL){
					stmtList->addTail(rstmt);
				}
			}
		}
		while(stmts->moveNext());
	}

	return stmtList;
}
*/

std::vector<RFSMstmt*>* RTObjBuilder::checkActionStmts(ErrorHandler& eHandler, EList<Gstmt*>* stmts)
{
	if(stmts == 0) return 0;

	std::vector<RFSMstmt*>* stmtList = NULL;
	
	if(stmts != 0 && stmts->moveHead()){
		stmtList = new std::vector<RFSMstmt*>();
		do{			
			Gstmt* stmt = stmts->getCurObject();
			RFSMstmt* rstmt = checkActionStmt(eHandler, stmt);

			if(rstmt == NULL){
				//errror STATEMENT에 올수 없는 것이 왔다.
				//eHandler.addErrorCount(); 에러를 발생하면 안된다.  위에서 type==SYMB인 경우가 있기 때문
			}
			else{														
				stmtList->push_back(rstmt);
			}
			
		}
		while(stmts->moveNext());
	}

	return stmtList;
}

RFSMstmt* RTObjBuilder::checkActionStmt(ErrorHandler& eHandler, Gstmt* stmt)
{
	RFSMstmt* rstmt = NULL;
	
	//statement 종류 5가지
	if(stmt !=0){
		/*
		if(typeid(*stmt)==typeid(GstmtIf) ){
			rstmt = checkIfstmt(eHandler, (GstmtIf*)stmt, false);
		}
		else*/ 
		rstmt = checkCommonStmts(eHandler, stmt, false);
		if(rstmt != NULL){
		}
		else if(typeid(*stmt)==typeid(Gwaitfor)){
			Gwaitfor* wf = (Gwaitfor*)stmt;
			rstmt = checkWaitfor(eHandler, wf);
		}	
		else if(typeid(*stmt)==typeid(Gsynch)){
			Gsynch* wf = (Gsynch*)stmt;
			rstmt = checkSynch(eHandler, wf);
			//RFSMbehavior에 synch id 정보 입력
			m_curTreeNode->addSynchID(((RFSMsysSynch*)rstmt)->getSynchID());
		}	
		else if(typeid(*stmt)==typeid(GstmtCall)){
			GstmtCall* cstmt = (GstmtCall*)stmt;
			Gtoken* name = cstmt->getCall()->getName();
			std::string nameStr = name->getData();

			//int result1 = m_symT->findAction(nameStr);
			int result2 = m_symT->findFunction(nameStr);
			int result3 = m_symT->findBehavior(nameStr);

			////////////////////////////////////////////////////////////////
			//오류 처리 및 보고
			if(result3 == 0){
				eHandler.addErrorCount();
				printErrMsg(TASK_IN_ACTION_BL_ERROR, name);
			}
			else if(result2 != 0){//검색 안된 경우
				eHandler.addErrorCount();
				//printErrMsg(result1, name);
				printErrMsg(result2, name);
			}
			/////////////////////////////////////////////////////////////////

			//if(result1 == 0)rstmt = checkActionCallstmt(eHandler, cstmt);
			if(result2 == 0){
				rstmt = checkFuncCallstmt(eHandler, cstmt);
			}
			//else if(result3 == 0)rstmt = checkTaskCallstmt(eHandler, cstmt);
		}
/*
		else if(typeid(*stmt)==typeid(GstmtExpr)){
			GstmtExpr* expr = (GstmtExpr*)stmt;
			ETree<Gtoken>* rtree = expr->getExprTree();
			ETree<Gtoken>* c = rtree->getFirstChild();
			Gtoken cNode = c->getNode();
			unsigned int type = cNode.getType();
			if(type==ASMT){
				rstmt = checkASMTExpr(eHandler, c);
			}
			if(type==SYMB){
				rstmt = new RFSMsymb(cNode.getData());
				//a; 문장은 단순히 a의 선언 여부만 검사
				checkSYMBExpr(eHandler, c);
			}

			//checkExpr(eHandler, (GstmtExpr*)stmt); //블럭에서 assingment 물론 향후 다른 것도 float a;
		}
*/				
		else if(typeid(*stmt)==typeid(GstmtGotoState)){
			//action block에 goto는 오면 안된다.에러처리
			GstmtGotoState* gt = (GstmtGotoState*)stmt;
			Gtoken* name = gt->getStateName();
			eHandler.addErrorCount();
			printErrMsg(GOTO_IN_ACTION_BL_ERROR, name);
		}
		
/*
		else if(typeid(*stmt)==typeid(GlocalVar)){//변수선언
			GlocalVar* var = (GlocalVar*)stmt;
			rstmt = checkLocalVarDecl(eHandler, var, blockID);
		}

		else if(typeid(*stmt)==typeid(Gwaitfor)){
			Gwaitfor* wf = (Gwaitfor*)stmt;
			//rstmt = new RFSMwait();
		}
*/
		else if(typeid(*stmt)==typeid(GstmtBlock)||typeid(*stmt)==typeid(GstateDo)||
			typeid(*stmt)==typeid(GstateEntry)||typeid(*stmt)==typeid(GstateExit)){
			GstmtBlock* sblock = (GstmtBlock*)stmt;
			int period = 10;
			//std::string bid = blockID;
			//bid.append(".").append(sblock->getBlockID());
			int mod = sblock->getModifier();
//printf("action mode : %d\n", mod);	
			std::vector<RFSMstmt*>* stmts = checkActionStmts(eHandler, sblock->getStmts());
			RFSMStmtBlock* bs;
			std::string clstype = CLS_RFSMblockStmt;
			if(mod == STMTBLOCK){		
				clstype = CLS_RFSMblockStmt;
				//bs = new RFSMStmtBlock(CLS_RFSMblockStmt, sblock->getFileName());					
			}
			if(mod == PAR){
				clstype = CLS_RFSMparStmt;
				//bs = new RFSMStmtBlock(CLS_RFSMparStmt, sblock->getFileName());	
			}
			if(mod == SEQ){
				clstype = CLS_RFSMseqStmt;
				//bs = new RFSMStmtBlock(CLS_RFSMseqStmt, sblock->getFileName());	
			}
			if(mod == WITH){
				clstype = CLS_RFSMwithStmt;
				//bs = new RFSMStmtBlock(CLS_RFSMwithStmt, sblock->getFileName());	
			}
			bs = new RFSMStmtBlock(clstype, sblock->getFileName(), sblock->getBlockPath(), sblock->getBlockName(), period, sblock->getEndLine());
			bs->setStmtBlock(stmts);
			rstmt = (RFSMstmt*)bs;
		}

		if(rstmt != NULL){
			rstmt->setEndLine(stmt->getEndLine());
			rstmt->setBlockID(stmt->getBlockPath());
		}
		else{
			eHandler.addErrorCount();
			Gtoken* tok = new Gtoken(stmt->getFileName(), stmt->getBlockName(), stmt->getBlockPath(), stmt->getStmtName(), stmt->getEndLine(), -1);
			printErrMsg(WRONG_STMT_IN_ACTION_BLOCK, tok);
			delete tok;
			
		}
	}
	return rstmt;
}


void RTObjBuilder::checkDeterminism(ErrorHandler& eHandler, Gstmt* stmt, int& num)
{
	if(stmt == 0) return;
	bool epdstmtAppeared = false;
	if(typeid(*stmt)==typeid(GstmtBlock)){
		GstmtBlock* sblock = (GstmtBlock*)stmt;
		EList<Gstmt*>* ss = sblock->getStmts();
		if(ss != 0 && ss->getSize()>0 && ss->moveHead()){
			do{			
				Gstmt* s = ss->getCurObject();

				//1. expand문 다음에 stmt가 오는지 검사
				if(epdstmtAppeared){
					eHandler.addErrorCount();	
					Gtoken* tok = new Gtoken(s->getFileName(), s->getBlockName(), s->getBlockPath(), s->getStmtName(), s->getEndLine(), -1);
					printErrMsg(UNREACHABLE_STMT_ERROR, tok);
					delete tok;
				}

				//2. expand문이 하나만 선택되게 하는 지를 검사
				checkDeterminism(eHandler, s, num);

				//expand문을 나타났음을 표식
				if(typeid(*s)==typeid(GstmtInvokeSub)) epdstmtAppeared = true;
			}
			while(ss->moveNext());
		}
	}
	if(typeid(*stmt)==typeid(GstmtIf) ){
		 GstmtIf* ifstmt = (GstmtIf*)stmt;
		
		 int num1 = 0;
		 int num2 = 0;
		//if(true){ifActs}
		Gstmt* ifActs = ifstmt->getIfAction();
		if(ifActs != 0) checkDeterminism(eHandler, ifActs, num1);
			
		//if(true){}else{elseBlock}
		Gstmt* elseBlock = ifstmt->getElse();
		if(elseBlock != NULL) checkDeterminism(eHandler, elseBlock, num2);
		
		if(num1 == 1 && num2 == 1) num = num + 1;
		if(num1 == 1 && num2 < 1) num = num + 1;
		if(num1 < 1 && num2 == 1) num = num + 1;

		if(num1 > 1 || num2 > 1)  num = num + 2;
	}

	if(typeid(*stmt)==typeid(GstmtInvokeSub)){
		num++;
	}

	if(num>1){
		eHandler.addErrorCount();				
		Gtoken* tok = new Gtoken(stmt->getFileName(), stmt->getBlockName(), stmt->getBlockPath(), stmt->getStmtName(), stmt->getEndLine(), -1);
		printErrMsg(NON_DETERMINISTIC_LINK_ERROR, tok);
		num = 0; //에러가 하나만 나오도록 하기 위해서
		delete tok;
	}
}

//if 조건 뒤에 오는 stmtblock
std::vector<RFSMstmt*>* RTObjBuilder::checkTransStmts(ErrorHandler& eHandler, EList<Gstmt*>* stmts, bool isRunBlock)
{
	if(stmts == 0) return 0;

	std::vector<RFSMstmt*>* stmtList = NULL;	
	if(stmts != 0 && stmts->getSize()>0 && stmts->moveHead()){
		stmtList = new std::vector<RFSMstmt*>();
		do{			
			Gstmt* stmt = stmts->getCurObject();
			RFSMstmt* rstmt = checkTransStmt(eHandler, stmt, isRunBlock);
			
			if(rstmt == NULL){
				//error STATEMENT에 올수 없는 것이 왔다
			}
			else{					
				stmtList->push_back(rstmt);
			}			
		}
		while(stmts->moveNext());
	}
	return stmtList;
}

RFSMstmt* RTObjBuilder::checkTransStmt(ErrorHandler& eHandler, Gstmt* stmt, bool isRunBlock)
{
	RFSMstmt* rstmt = NULL;

	//checkStmt(eHandler, stmt);
	if(stmt !=0){
		/*
		if(typeid(*stmt)==typeid(GstmtIf) ){
			rstmt = checkIfstmt(eHandler, (GstmtIf*)stmt, true);
		}
		else */

		rstmt = checkCommonStmts(eHandler, stmt, isRunBlock);

		if(rstmt != NULL){
		}
		else if(typeid(*stmt)==typeid(GstmtGotoState)){
			GstmtGotoState* gs = (GstmtGotoState*)stmt;					
			rstmt = checkGoto(eHandler, gs->getStateName());
		}
		else if(typeid(*stmt)==typeid(GstmtInvokeSub)){
			GstmtInvokeSub* cs = (GstmtInvokeSub*)stmt;
			Gcall* ivk = cs->getInvokeCall();
			Gtoken* name = ivk->getName(); //호출문
		
			int result1 = m_symT->findBehavior(name->getData());				
			int result3 = m_symT->findConnector(name->getData());
			//int result3 = m_symT->findAction(name->getData());

			////////////////////////////////////////////////////////////////
			//오류 처리 및 보고					
			if(result1 !=0 && result3 !=0){ //존재하지 않는 이름 사용
				eHandler.addErrorCount();						
				//printErrMsg(result1, name);					
				//printErrMsg(result3, name);
				printErrMsg(SYM_BEHA_N_CONN_NOT_FOUND, name);
			}			
			/////////////////////////////////////////////////////////////////

			//오류가 아닐 때
			if(result1 == 0){ //behavior인경우
//if문의 경우 if조건에서 m_curTreeNode가 설정되고 else조건에서 root가 바뀌어야 한다.
//그렇기 때문에 checkTransStmts에서 파라메터로 m_curTreeNode가 입력되어야 한다.

	//m_curTreeNode->addLinkedNode(name->getData()); //for dot
	//m_curState->addLinkedBhv(name->getData());// for dot
				rstmt = checkBhvCallstmt(eHandler, cs, isRunBlock);		
				m_curTreeNode->addLinkedNode(((RFSMcall*)rstmt)->getName(), BEHAVIOR);
			}				
			else if(result3 ==0){//connector인 경우
				rstmt = checkConnCallstmt(eHandler, cs, isRunBlock);		
				m_curTreeNode->addLinkedNode(((RFSMcall*)rstmt)->getName(), CONNECTOR);
			}
		}
		else if(typeid(*stmt)==typeid(GstmtCall)){
			GstmtCall* cstmt = (GstmtCall*)stmt;
			Gtoken* name = cstmt->getCall()->getName();
			int result2 = m_symT->findFunction(name->getData());

			////////////////////////////////////////////////////////////////
			//오류 처리 및 보고
			if(result2 ==0){ //함수인 경우
				TABfunction* tfunc = m_symT->getFunction(name->getData());
				if(tfunc->isAction()){ //혹시 Action인지를 검사
					eHandler.addErrorCount();
					printErrMsg(ACTION_IN_TRAN_BL_ERROR, name);
				}
			}
			else if(result2 != 0){ //존재하지 않는 이름 사용
				eHandler.addErrorCount();					
				printErrMsg(result2, name);						
			}
			////////////////////////////////////////////////////////////////
			if(result2 == 0){
				rstmt = checkFuncCallstmt(eHandler, cstmt);
			}
		}
	/*	else if(typeid(*stmt)==typeid(GstmtExpr)){
			GstmtExpr* expr = (GstmtExpr*)stmt;
			ETree<Gtoken>* rtree = expr->getExprTree(); //EXPR_ROOT
			ETree<Gtoken>* c = rtree->getFirstChild();
			Gtoken cNode = c->getNode();
			unsigned int type = cNode.getType();
			if(type==ASMT){
				rstmt = checkASMTExpr(eHandler, c);
			}
			if(type==SYMB){//그냥 문자열이 오는 경우 ex) var1;
				rstmt = new RFSMsymb(cNode.getData());
				checkSYMBExpr(eHandler, c);
			}
			//checkExpr(eHandler, (GstmtExpr*)stmt); //블럭에서 assingment 물론 향후 다른 것도 float a;
		}
		else if(typeid(*stmt)==typeid(GlocalVar)){//변수선언
			GlocalVar* var = (GlocalVar*)stmt;
			rstmt = checkLocalVarDecl(eHandler, var, blockID);
		}

		else if(typeid(*stmt)==typeid(Gwaitfor)){
			Gwaitfor* wf = (Gwaitfor*)stmt;
			//rstmt = new RFSMwait();
		}
        */
		else if(typeid(*stmt)==typeid(GstmtBlock)){
			GstmtBlock* sblock = (GstmtBlock*)stmt;
			int period = 10;
			//std::string bid = blockID;
			//bid.append(".").append(sblock->getBlockID());
			int mod = sblock->getModifier();
			std::vector<RFSMstmt*>* stmts = checkTransStmts(eHandler, sblock->getStmts(), isRunBlock); 
			RFSMStmtBlock* bs;
			std::string clstype = CLS_RFSMblockStmt;
			if(mod == STMTBLOCK){	
				clstype = CLS_RFSMblockStmt;
				//bs = new RFSMStmtBlock(CLS_RFSMblockStmt, sblock->getFileName());					
			}
			if(mod == PAR){
				clstype = CLS_RFSMparStmt;
				//bs = new RFSMStmtBlock(CLS_RFSMparStmt, sblock->getFileName());	
			}
			if(mod == WITH){
				clstype = CLS_RFSMwithStmt;
				//bs = new RFSMStmtBlock(CLS_RFSMwithStmt, sblock->getFileName());	
			}
			bs = new RFSMStmtBlock(clstype, sblock->getFileName(), sblock->getBlockPath(), sblock->getBlockName(), period, sblock->getEndLine());
			bs->setStmtBlock(stmts);
			rstmt = (RFSMstmt*)bs;
		}

		if(rstmt != NULL){
			rstmt->setEndLine(stmt->getEndLine());
			rstmt->setBlockID(stmt->getBlockPath());
		}
		else{
			eHandler.addErrorCount();
			Gtoken* tok = new Gtoken(stmt->getFileName(), stmt->getBlockName(), stmt->getBlockPath(), stmt->getStmtName(), stmt->getEndLine(), -1);
			printErrMsg(WRONG_STMT_USED_IN_DECI_BLOCK, tok);
			delete tok;
			
		}
	}
	return rstmt;
}

RFSMstmt* RTObjBuilder::checkCommonStmts(ErrorHandler& eHandler, Gstmt* stmt, bool isRunBlock)
{
	RFSMstmt* rstmt = NULL;

	if(typeid(*stmt)==typeid(GstmtIf) ){
		rstmt = checkIfstmt(eHandler, (GstmtIf*)stmt, true, isRunBlock);
	}
	else if(typeid(*stmt)==typeid(GstmtExpr)){
		GstmtExpr* expr = (GstmtExpr*)stmt;
		EXTree* rtree = expr->getExprTree(); //EXPR_ROOT
		EXTree* c = rtree->getFirstChild();
		Gtoken cNode = c->getNode();
		unsigned int type = cNode.getType();
		if(type==ASMT){
			rstmt = checkASMTExpr(eHandler, c);
		}
		if(type==SYMB){//그냥 문자열이 오는 경우 ex) var1;
			rstmt = new RFSMsymb(cNode.getData(), stmt->getFileName(), stmt->getBlockPath(), stmt->getBlockName(), stmt->getEndLine());			
			checkSYMBExpr(eHandler, c);
		}		
		//checkExpr(eHandler, (GstmtExpr*)stmt); //블럭에서 assingment 물론 향후 다른 것도 float a;
	}
	else if(typeid(*stmt)==typeid(GlocalVar)){//변수선언
		GlocalVar* var = (GlocalVar*)stmt;
		rstmt = checkLocalVarDecl(eHandler, var);
	}

	return rstmt;
}

/*
RFSMstmt* RTObjBuilder::checkStmt(ErrorHandler& eHandler, Gstmt* stmt)
{
	RFSMstmt* rstmt = NULL;

	if(typeid(*stmt)==typeid(GstmtIf) ){
		rstmt = checkIfstmt(eHandler, (GstmtIf*)stmt, true);
	}

	else if(typeid(*stmt)==typeid(GstmtExpr)){
		GstmtExpr* expr = (GstmtExpr*)stmt;
		EXTree* rtree = expr->getExprTree(); //EXPR_ROOT
		EXTree* c = rtree->getFirstChild();
		Gtoken cNode = c->getNode();
		unsigned int type = cNode.getType();
		if(type==ASMT){
			rstmt = checkASMTExpr(eHandler, c);
		}
		if(type==SYMB){//그냥 문자열이 오는 경우 ex) var1;
			rstmt = new RFSMsymb(cNode.getData(), stmt->getFileName());			
			checkSYMBExpr(eHandler, c);
		}		
		//checkExpr(eHandler, (GstmtExpr*)stmt); //블럭에서 assingment 물론 향후 다른 것도 float a;
	}

	else if(typeid(*stmt)==typeid(GlocalVar)){//변수선언
		GlocalVar* var = (GlocalVar*)stmt;
		rstmt = checkLocalVarDecl(eHandler, var);
	}

	else if(typeid(*stmt)==typeid(Gwaitfor)){
		Gwaitfor* wf = (Gwaitfor*)stmt;
		rstmt = checkWaitfor(eHandler, wf);
	}	

	else if(typeid(*stmt)==typeid(GstmtGotoState)){
		GstmtGotoState* gs = (GstmtGotoState*)stmt;					
		rstmt = checkGoto(eHandler, gs->getStateName());
	}

	else if(typeid(*stmt)==typeid(GstmtInvokeSub)){
		GstmtInvokeSub* cs = (GstmtInvokeSub*)stmt;
		Gcall* ivk = cs->getInvokeCall();
		Gtoken* name = ivk->getName(); //호출문
	
		int result1 = m_symT->findBehavior(name->getData());				
		int result3 = m_symT->findConnector(name->getData());
		//int result3 = m_symT->findAction(name->getData());

		////////////////////////////////////////////////////////////////
		//오류 처리 및 보고					
		if(result1 !=0 && result3 !=0){ //존재하지 않는 이름 사용
			eHandler.addErrorCount();						
			printErrMsg(SYM_BEHA_N_CONN_NOT_FOUND, name);
		}			
		/////////////////////////////////////////////////////////////////

		//오류가 아닐 때
		if(result1 == 0){ //behavior인경우
			rstmt = checkBhvCallstmt(eHandler, cs);		
			m_curTreeNode->addLinkedNode(((RFSMcall*)rstmt)->getName());
		}				
		else if(result3 ==0){//connector인 경우
			rstmt = checkConnCallstmt(eHandler, cs);		
			m_curTreeNode->addLinkedNode(((RFSMcall*)rstmt)->getName());
		}
	}

	return rstmt;
}
*/

RFSMsysWaitfor* RTObjBuilder::checkWaitfor(ErrorHandler& eHandler, Gwaitfor* wf)
{
	RFSMsysWaitfor* rwf = new RFSMsysWaitfor("SLEEP", wf->getFileName(), wf->getBlockPath(), wf->getBlockName(), wf->getEndLine());

	GstmtExpr* expr = wf->getTime();
	EXTree* rtree = expr->getExprTree();		//EXPR_ROOT
	EXTree* cparam = rtree->getFirstChild();		//CPARAM 노드

	//반드시 assignment문이야 한다.
	//RFSMvalue* rParam = checkCPARAMExpr(eHandler, cparam);
	RFSMvalue* val = checkVALUEExpr(eHandler, cparam);

	rwf->setValue(val);
	
	return rwf;
}

RFSMsysSynch* RTObjBuilder::checkSynch(ErrorHandler& eHandler, Gsynch* syn)
{
	RFSMsysSynch* rSyn = new RFSMsysSynch("SYNCH", syn->getFileName(), syn->getBlockPath(), syn->getBlockName(), syn->getEndLine());

	GstmtExpr* expr = syn->getSynchID();
	EXTree* rtree = expr->getExprTree();		//EXPR_ROOT
	EXTree* cparam = rtree->getFirstChild();		//CPARAM 노드

	//반드시 assignment문이야 한다.
	//RFSMvalue* rParam = checkCPARAMExpr(eHandler, cparam);
	RFSMvalue* val = checkVALUEExpr(eHandler, cparam);

	rSyn->setValue(val);
	
	return rSyn;
}


RFSMstmt* RTObjBuilder::checkLocalVarDecl(ErrorHandler& eHandler, GlocalVar* var)
{
	Gtoken* ntok = var->getName();
	Gtoken* ttok = var->getType();
	unsigned int vtype = getType(ttok->getData());
	if(vtype == TYP_NOT_PRIMITIVE)
		vtype = m_symT->getUserTypeID(ttok->getData());
	if(vtype == UNDEFINED_TYPE){
		eHandler.addErrorCount();
		printErrMsg(vtype, ttok);
		return NULL;
	}

	int result = m_symT->checkMoldelDuplication(ntok->getData());

	if(result == SYM_NOT_DUPLICATED){//없으면
		RFSMvariable* lvariable = new RFSMvariable(ntok->getBlockPath(), var->getBlockName(), ntok->getData(), vtype, G_LOCAL, ntok->getFileName(), ntok->getEndLine());
		TABsymbol* rsym = new TABsymbol(ntok->getBlockPath(), var->getBlockName(), ntok->getData(),vtype, 100, "");
		int r2 = m_symT->addLocalVar(ntok->getBlockPath(), rsym);
		if(r2==SYM_LOCAL_VAR_DUPLICATED){
			eHandler.addErrorCount();
			printErrMsg(LOCALVAR_NAME_DUPLICATED, ntok);
		}
		else{
			GstmtExpr* vexpr = var->getValueExpr(); //선언과 동신에 할당
			if(vexpr != NULL){
				EXTree* rc = vexpr->getExprTree();
				RFSMvalue* rval = checkVALUEExpr(eHandler, rc->getFirstChild()); //EXPR_ROOT다음

				if(rval != NULL){//함수이름, 심벌등이 정의 되지않은 등의 이유로 에러가 나면 NULL;
					int result = assignable(lvariable->getType(), rval->getType());
					if(result != 0){
						eHandler.addErrorCount();
						printErrMsg(result, ntok);
						return NULL;
					}
					//할당문과 같은 의미이므로 할당문으로 리턴
					RFSMasmt* rasmt = new RFSMasmt(OP_ASI, lvariable, rval, vexpr->getFileName(), vexpr->getEndLine());			
					return rasmt;
				}
			}
			else{//그냥 변수 선언
				RFSMvar* rvar = new RFSMvar(OP_ASI, lvariable, var->getFileName(), var->getEndLine());	
				return rvar;
			}
		}
	}
	else{
		eHandler.addErrorCount();									
		//printErrMsg(result, ntok);		
		printErrMsg(LOCALVAR_NAME_DUPLICATED, ntok);
	}

	return NULL;
}

/*
void RFSMsemantics::checkStmt(ErrorHandler& eHandler, Gstmt* stmt)
{
	//const char* a = typeid(GstmtIf).name();
	//const char* b = typeid(stmt).name();
	//const char* c = typeid(*stmt).name();
	if(stmt ==0) return;

	if(typeid(*stmt)==typeid(GstmtIf) ){
		checkIfstmt(eHandler, (GstmtIf*)stmt);
	}
	else if(typeid(*stmt)==typeid(GstmtGotoState)){
		checkGoto(eHandler, (GstmtGotoState*)stmt);
	}
	else if(typeid(*stmt)==typeid(GstmtCall)){
		checkFuncall(eHandler, (GstmtCall*)stmt);
	}
	else if(typeid(*stmt)==typeid(GstmtExpr)){
		checkExpr(eHandler, (GstmtExpr*)stmt); //블럭에서 assingment
	}
}
*/

RFSMifStmt* RTObjBuilder::checkIfstmt(ErrorHandler& eHandler, GstmtIf* ifstmt, bool inTrans, bool isRunBlock)
{
	if(ifstmt == 0)return 0;

	RFSMifStmt* rifStmt = new RFSMifStmt(ifstmt->getFileName(), ifstmt->getBlockPath(), ifstmt->getBlockName(), ifstmt->getEndLine());

	// if(ifcond)
	GstmtExpr *ifCond = ifstmt->getIfCond()->getCondition();
	if(ifCond != 0){

		EXTree* rtree = ifCond->getExprTree();
		EXTree* c = rtree->getFirstChild();//EXPR_ROOT

		RFSMboolable* rcond = checkLOGICALExpr(eHandler, c);
		rifStmt->setCondition(rcond);
	}
	else{
		//error
	}

	//if(true){ifActs}
	Gstmt* ifActs = ifstmt->getIfAction();

	if(ifActs != 0){
		RFSMstmt* tab = NULL;
		if(inTrans)
			tab = checkTransStmt(eHandler, ifActs, isRunBlock);
		else
			tab = checkActionStmt(eHandler, ifActs);
		
		if(tab != NULL){
				std::vector<RFSMstmt*>* a = new std::vector<RFSMstmt*>();
				a->push_back(tab);
				rifStmt->setTrueStmts(a);
		}
		//rifStmt->setTrueStmts(tab);

		//if(true){}else{elseBlock}
		Gstmt* elseBlock = ifstmt->getElse();
		if(elseBlock != NULL){
			RFSMstmt* fab = NULL;
			if(inTrans)
				fab = checkTransStmt(eHandler, elseBlock, isRunBlock);
			else
				fab = checkActionStmt(eHandler, elseBlock);

			if(fab != NULL){
				std::vector<RFSMstmt*>* a = new std::vector<RFSMstmt*>();
				a->push_back(fab);
				rifStmt->setFalseStmts(a);
			}
			//rifStmt->setFalseStmts(fab);
		}
	}

	return rifStmt;
}

RFSMboolable* RTObjBuilder::checkLOGICALExpr(ErrorHandler& eHandler, EXTree* expr)
{
	//숫자가 올수도 있나???
	//java는 확실히 정수와 boolean을 구별한다 ...
	//C는 정수와 bool(+정수로 간주)을 구별하지 않는다.
	//여기서는 확실히 구별한다. java처럼


	if(expr==NULL) return NULL;

	Gtoken cNode = expr->getNode();
	unsigned int type = cNode.getType();

	//NOT, AND, OR, BOOL로 시작
	if(type==NOT){
		//현재 DNF로 변환후 이과정을 거치므로 NOT은 없다.. 단항은 not이 없어지지 않는다.
		//나중에 DNF이전으로 해야할 듯
		RFSMboolable* boolable = checkLOGICALExpr(eHandler, expr->getFirstChild());
		boolable->setHasNot();
		return boolable;
	}

	if(type==AND){
		RFSMand* andp = checkANDExpr(eHandler, expr);
		return andp;
	}

	if(type==OR){
		RFSMor* orp = checkORExpr(eHandler, expr);
		return orp;
	}

	if(type==BOOL){ //관계연산 <,>,==, !=. >=,<=
		RFSMRelational* relational = checkRelExpr(eHandler, expr);
		return relational;
	}

	if(type==SYMB){

		EXTree* c2 = expr->getFirstChild();
		Gtoken sym = c2->getNode();

		RFSMsym* rSym = checkSYMBExpr(eHandler, expr);
		RFSMvariable* rVar = NULL;
		if(rSym != NULL){
			if(typeid(*rSym)==typeid(RFSMvariable)){

				unsigned int rType = rSym->getType();
				//symbol이 반드시 bool타입
				if(rType != TYP_BOOL){
					eHandler.addErrorCount();
					printErrMsg(SYM_TYPE_MISS_MATCHED, &sym);
				}
				else{
					rVar = (RFSMvariable*)rSym;
				}
			}
			else{
				eHandler.addErrorCount();
				//enum인경우
				printErrMsg(SYM_TYPE_MISS_MATCHED, &sym);
			}
		}

		return rVar;
	}

	if(type==LITE){//true나 false 같은 문자열이 조건문에 올수 있다.

		EXTree* c2 = expr->getFirstChild();
		Gtoken lite = c2->getNode();
		int type = lite.getType();

		RFSMliteral* li = checkLITEExpr(eHandler, expr);//이걸 호출할 필요가 있나?
		if(type != TYP_BOOL){
			eHandler.addErrorCount();
			printErrMsg(SYM_TYPE_MISS_MATCHED, &lite);
		}

		return li;
	}

	if(type==CALL){ //if(함수하나)

		EXTree* cname = expr->getFirstChild();	//CNAME
		EXTree* id = cname->getFirstChild();  //함수명
		Gtoken nNode = id->getNode();

		RFSMcallFunc* func = checkFUNCExpr(eHandler, expr);
		if(func != 0){
			unsigned int rType = func->getType();
			//return 타입이 반드시 bool타입
			if(rType != TYP_BOOL){
				eHandler.addErrorCount();
				printErrMsg(SYM_TYPE_MISS_MATCHED, &nNode);
			}
		}

		return func;
	}

#ifdef __DEBUG_SYMBOL_
std::cout<<"\n";
#endif

	eHandler.addErrorCount();
	printErrMsg(NOT_BOOL_EXPR, &cNode);

	return NULL;

}

RFSMcallFunc* RTObjBuilder::checkFuncCallstmt(ErrorHandler& eHandler, GstmtCall *func)
{
	if(func == NULL) return NULL;

	//리턴 값
	RFSMcallFunc* rfunc = NULL;

	//토큰으로 부터 함수 이름 추출
	Gtoken* name = func->getCall()->getName();
	std::string callName = name->getData();

#ifdef __DEBUG_SYMBOL_
std::cout<<"CALL "<<name->getData().c_str()<<" \n";
#endif

	//함수가 존재하는지 검사
	int result = m_symT->findFunction(callName);	//function호출

	if(result !=0){ //함수가 존재하지 않음
		eHandler.addErrorCount();
		printErrMsg(result, name);
	}
	else{
		//함수 정보 획득
		TABfunction* funInfo = m_symT->getFunction(callName);

		//리턴할 RFSMcallFucn에 데이터 설정		
		rfunc = new RFSMcallFunc(name->getBlockPath(), name->getBlockName(), callName, funInfo->isAction(), func->getFileName(), func->getEndLine());
		rfunc->setRtnType(funInfo->getRtnType());

		//파라메터 검사
		EList<GstmtExpr*>* params = func->getCall()->getParameters();
		//checkFuncActionParam(eHandler, name, params, tfunc, rfunc);
		checkCallParams(eHandler, name, params, funInfo, rfunc);
	}
	return rfunc;
}

RFSMcallBhv* RTObjBuilder::checkBhvCallstmt(ErrorHandler& eHandler, GstmtInvokeSub *ivkSub, bool isRunBlock)
{
	if(ivkSub == NULL) return NULL;

	Gcall* func = ivkSub->getInvokeCall();
	Gtoken* name = func->getName();
	Gtoken* nextTarget = ivkSub->getNextTarget(); //~>
	

#ifdef __DEBUG_SYMBOL_
std::cout<<"CALL "<<name->getData().c_str()<<" \n";
#endif

	RFSMcallBhv* rBhv = NULL;

	std::string callName = name->getData();
	int result = m_symT->findBehavior(callName);		
	if(result != 0){
		//error처리는 checkTransStmts에서 이미 한다. 그러므로 아래 것은 중복
		//eHandler.addErrorCount();
		//printErrMsg(result, name);		
	}
	else{		
		std::string ntname = "";
		if(nextTarget != NULL && !isRunBlock) {
			RFSMgotoStmt* rntarget = checkGoto(eHandler, nextTarget);
			if(rntarget != NULL){
				ntname = rntarget->getName();
				delete rntarget;
			}
		}
		if(nextTarget != NULL && isRunBlock){			
			if(nextTarget->getData() != "break"){ 
				result = WRONG_SYMBOL_USED_IN_EXPAND_STMT;
				eHandler.addErrorCount();
				printErrMsg(result, nextTarget);		
			}
		}
		
		rBhv = new RFSMcallBhv(callName, ntname, func->getFileName(), func->getBlockPath(), func->getBlockName(), ivkSub->getPeriod(), func->getEndLine());

		EList<GstmtExpr*>* params = func->getParameters();

		TABbehavior* ttask = m_symT->getBehavior(callName);

		//checkFuncActionParam(eHandler, name, params, tfunc, rfunc);
		checkCallParams(eHandler, name, params, ttask, rBhv);
	}
	return rBhv;
}


RFSMcallConnector* RTObjBuilder::checkConnCallstmt(ErrorHandler& eHandler, GstmtInvokeSub *ivkSub, bool isRunBlock)
{
	if(ivkSub == NULL) return NULL;

	Gcall* func = ivkSub->getInvokeCall();
	Gtoken* name = func->getName();
	Gtoken* nextTarget = ivkSub->getNextTarget();

#ifdef __DEBUG_SYMBOL_
std::cout<<"CALL "<<name->getData().c_str()<<" \n";
#endif

	RFSMcallConnector* rcon= NULL;

	std::string callName = name->getData();
	int result = m_symT->findConnector(callName);		
	if(result != 0){//존재하지 않음
		//error처리는 checkTransStmts에서 이미 한다. 
	}
	else{		
		
		//~>오류검사
		std::string ntname = "";
		if(nextTarget != NULL && !isRunBlock) {
			RFSMgotoStmt* rntarget = checkGoto(eHandler, nextTarget);
			if(rntarget != NULL){
				ntname = rntarget->getName();
				delete rntarget;
			}
		}
		if(nextTarget != NULL && isRunBlock){			
			if(nextTarget->getData() != "break"){ 
				result = WRONG_SYMBOL_USED_IN_EXPAND_STMT;
				eHandler.addErrorCount();
				printErrMsg(result, nextTarget);
			}
	
		}
		rcon = new RFSMcallConnector(callName, ntname, func->getFileName(), func->getBlockPath(), func->getBlockName(), ivkSub->getPeriod(), func->getEndLine());

		EList<GstmtExpr*>* params = func->getParameters();

		TABconnector * tcon = m_symT->getConnector(callName);

		//checkFuncActionParam(eHandler, name, params, tfunc, rfunc);
		checkCallParams(eHandler, name, params, tcon, rcon);
	}
	return rcon;
}
/**
Function/Action/Task 호출시에 매개변수를 검사한다.
name은 함수 이름 정보를 담고 있는 토큰
params 함수의 파라메터들
funcInfo 심볼테이블에 있는 함수 정보
rcall 현재 체크 중인 함수 객체(리턴할 실행객체)
*/
void RTObjBuilder::checkCallParams(ErrorHandler& eHandler, Gtoken* name, EList<GstmtExpr*>* params, TABcall* funcInfo, RFSMcall* rcall)
{
	if(params != 0 && params->getSize()>0 && params->moveHead()){
		do{
			GstmtExpr* expr = params->getCurObject();
			EXTree* rtree = expr->getExprTree();		//EXPR_ROOT
			EXTree* cparam = rtree->getFirstChild();		//CPARAM 노드

			//반드시 assignment문이야 한다.
			//RFSMvalue* rParam = checkCPARAMExpr(eHandler, cparam);

			checkCallParam(eHandler, name, cparam, funcInfo, rcall);
		}
		while(params->moveNext());
	}
	else{//호출시 파라메터가 없는 경우
		EList<TABparameter*>* tparams = funcInfo->getParameters();
		
		////////////////////////////////////////////////////////////////
		//오류 보고
		if(tparams!=0 && tparams->getSize()>0){//선언에는 파라메터가 있다.
			eHandler.addErrorCount();
			printErrMsg(SYM_FUNC_PARAM_FORM_WRONG, name);
		}
		////////////////////////////////////////////////////////////////
	}
}

/**
함수에 포함되는 하나의 파라메터를 체크
nNode: 에러메시지를 출력하기 위한 함수 이름를 담고 있는 token
*/
void  RTObjBuilder::checkCallParam(ErrorHandler& eHandler, Gtoken* nNode, EXTree* cparam, TABcall* funcInfo, RFSMcall* rcall)
{

	//ETree<Gtoken>* child = params->getCurrentChild(); //CParam
	EXTree* fc = cparam->getFirstChild();
	Gtoken node = fc->getNode();
	if(node.getType() != ASMT){
		eHandler.addErrorCount();
		printErrMsg(SYM_FUNC_PARAM_FORM_WRONG, nNode);
	}
	else{
		EXTree* op = fc->getFirstChild();
		EXTree* left = fc->getNextChild();
		EXTree* right = fc->getNextChild();

		////////////////////////////////////////////////////////////////
		//오류 보고
		Gtoken node2 = left->getNode();
		if(node2.getType() != SYMB){
			eHandler.addErrorCount();
			printErrMsg(SYM_LVALUE_MUSTBE_VAR, nNode);
		}
		///////////////////////////////////////////////////////////////

		Gtoken leftok = left->getFirstChild()->getNode();
		std::string lvar = leftok.getData();
		RFSMvalue* rfsmR = checkVALUEExpr(eHandler, right);
		TABparameter* tp = funcInfo->getParameter(lvar);
		if(tp == NULL){
			eHandler.addErrorCount();
			printErrMsg(SYM_FUNC_PARAM_NAME_WRONG, &leftok);
		}
		else{
			int pType = getType(tp->getType());
			rfsmR->setType(pType); // 파라메터의 선어 타입과 할당 값이 다를 수 있기 때문에 명시적으로 셋팅함 
			                       // double인 파라메터 v1=1을 할당하고 타입을 놔두면 문제가 됨
			RFSMparam* rfsmParam = new RFSMparam(lvar, rfsmR);
			rcall->addParameter(rfsmParam);
		}

	}
}

RFSMgotoStmt* RTObjBuilder::checkGoto(ErrorHandler& eHandler, Gtoken* sName)
{
	if(sName ==0) return 0;

	////////////////////////////////////////////////////////////
	//오류 검사
	if(sName == NULL){
		eHandler.addErrorCount();
		return NULL;
	}

	std::string sNameStr = sName->getData();
	int result = m_symT->findState(sNameStr);

	if(result != 0 && sNameStr.compare("stay")!=0){
		eHandler.addErrorCount();
		printErrMsg(result, sName);
		return NULL;
	}
	////////////////////////////////////////////////////////////

	RFSMgotoStmt* gotoStmt  = new RFSMgotoStmt(sName->getData(), sName->getFileName(), sName->getBlockPath(), sName->getBlockPath(), sName->getEndLine());
	m_curState->addLinkedstate(sNameStr);

#ifdef __DEBUG_SYMBOL_
	std::cout<<"GOTO "<<sName->getData().c_str()<<" \n";
#endif

	return gotoStmt;
}

/*
void RFSMsemantics::checkLOGICExpr(ErrorHandler& eHandler, ETree<Gtoken>* expr)
{
	Gtoken node = expr->getNode();
	int type = node.getType();

	if(type==AND){
		checkANDExpr(eHandler, expr);
	}
	if(type==OR){
		checkORExpr(eHandler, expr);
	}
	if(type==BOOL){
		checkRelExpr(eHandler, expr);
	}
	if(type==SYMB){ //bool타입의 symbol이어야 한다.
		checkSYMBExpr(eHandler, expr);
	}
}
*/

/**
* 이름이 좀 헤갈리지만 BOOLExpr은 관계연산을 의미한다.(>, <, == 등)
*/
RFSMRelational* RTObjBuilder::checkRelExpr(ErrorHandler& eHandler, EXTree* expr)
{
	if(expr ==0) return 0;

	Gtoken cNode = expr->getNode();

#ifdef __DEBUG_SYMBOL_
std::cout<<cNode.getData().c_str()<<" ";
#endif

	//operator
	EXTree* op = expr->getFirstChild();
	EXTree* opc = op->getFirstChild();
	Gtoken opNode = opc->getNode();
	std::string opstr = opNode.getData();

#ifdef __DEBUG_SYMBOL_
std::cout<<opNode.getData().c_str()<<" ";
#endif

	//left operand
	EXTree* lc = expr->getNextChild();
	RFSMvalue* l_val = checkVALUEExpr(eHandler, lc);

	//right operand
	EXTree* rc = expr->getNextChild();
	RFSMvalue* r_val = checkVALUEExpr(eHandler, rc);

	return new RFSMRelational(getOpType(opstr), l_val, r_val);
}


RFSMand* RTObjBuilder::checkANDExpr(ErrorHandler& eHandler, EXTree* expr)
{
	if(expr ==0) return 0;

	Gtoken cNode = expr->getNode();

#ifdef __DEBUG_SYMBOL_
std::cout<<cNode.getData().c_str()<<" ";
#endif

	RFSMand* andp = new RFSMand();
	if(expr->move2FirstChild()){
		do{
			EXTree* child = expr->getCurrentChild();
			RFSMboolable* boo = checkLOGICALExpr(eHandler, child);
			andp->addBoolExpr(boo);
		}
		while(expr->move2NextChild());
	}

	return andp;
}

RFSMor* RTObjBuilder::checkORExpr(ErrorHandler& eHandler, EXTree* expr)
{
	if(expr ==0) return 0;

	Gtoken cNode = expr->getNode();

#ifdef __DEBUG_SYMBOL_
std::cout<<cNode.getData().c_str()<<" ";
#endif

	RFSMor* orp = new RFSMor();
	if(expr->move2FirstChild()){
		do{
			EXTree* child = expr->getCurrentChild();
			RFSMboolable* boo = checkLOGICALExpr(eHandler, child);
			orp->addBoolExpr(boo);
		}
		while(expr->move2NextChild());
	}

	return orp;
}

RFSMvalue* RTObjBuilder::checkCPARAMExpr(ErrorHandler& eHandler, EXTree* expr)
{
	if(expr ==0) return 0;

	Gtoken cNode = expr->getNode();

#ifdef __DEBUG_SYMBOL_
std::cout<<cNode.getData().c_str()<<" ";
#endif

	EXTree* first = expr->getFirstChild();
	return checkVALUEExpr(eHandler, first);
}


RFSMvalue* RTObjBuilder::checkVALUEExpr(ErrorHandler& eHandler, EXTree* expr)
{
	Gtoken node = expr->getNode();
	int type = node.getType();

	if(type==ADD){
		return checkADDExpr(eHandler, expr);
	}
	if(type==MUL){
		return checkMULExpr(eHandler, expr);
	}
	if(type==SIGN){
		return checkSIGNExpr(eHandler, expr);
	}
	if(type==SYMB){
		return checkSYMBExpr(eHandler, expr);
	}
	if(type==LITE){
		return checkLITEExpr(eHandler, expr);
	}
	if(type==CALL){
		return checkFUNCExpr(eHandler, expr);
	}
	if(type==COND){ //(hasBall? a+100 : a-100)
		return checkCONDExpr(eHandler, expr);
	}
	if(type==ASMT){
		return checkASMTExpr(eHandler, expr);
	}

	return 0;
}

RFSMasmt* RTObjBuilder::checkASMTExpr(ErrorHandler& eHandler, EXTree* expr)
{
	if(expr == 0) return 0;

	Gtoken cNode = expr->getNode();

#ifdef __DEBUG_SYMBOL_
	std::cout<<cNode.getData().c_str()<<" ";
#endif

	//operator
	EXTree* op = expr->getFirstChild();
	EXTree* opc = op->getFirstChild();
	Gtoken opNode = opc->getNode();

#ifdef __DEBUG_SYMBOL_
	std::cout<<opNode.getData().c_str()<<" ";
#endif


	//left는 파라메터 이름이다.
	EXTree* lc = expr->getNextChild();
	RFSMvalue* lval = checkVALUEExpr(eHandler, lc);
	RFSMvariable* lvariable = NULL;

	//right operand, 변수나 값이나 상관없다.
	EXTree* rc = expr->getNextChild();
	RFSMvalue* rVal = checkVALUEExpr(eHandler, rc);

	if(lval != NULL){//symbol 테이블에 없으면 NULL이 리턴된다.
		//left operand, 반드시 변수 이어야 한다.

		if(typeid(*lval)==typeid(RFSMvariable) ){
			lvariable = (RFSMvariable*)lval;
			//할당가능한지 검사
			int result = assignable(lvariable->getType(), rVal->getType());
			if(result != 0){
				eHandler.addErrorCount();
				printErrMsg(result, &opNode);
				return NULL;
			}
		}
		else{
			eHandler.addErrorCount();
			Gtoken tok = lc->getFirstChild()->getNode();
			printErrMsg(SYM_LVALUE_MUSTBE_VAR, &tok);
			//lvariable = NULL;
			return NULL;
		}
	}
	else
		return NULL;


#ifdef __DEBUG_SYMBOL_
	std::cout<<std::endl;
#endif

	return new RFSMasmt(getOpType(opNode.getData()), lvariable, rVal, cNode.getFileName(), cNode.getEndLine());
}


RFSMcondValue* RTObjBuilder::checkCONDExpr(ErrorHandler& eHandler, EXTree* expr)
{
	if(expr == 0) return 0;

	//[ball.play_ball_precisely ? 400 : 300] 5개의 children

	EXTree* boolexp = expr->getFirstChild();
	//checkLOGICExpr(eHandler, boolexp);
	RFSMboolable* boo = checkLOGICALExpr(eHandler, boolexp);

	//? ->지나치기
	expr->getNextChild();

	//(*** ? A: B)에서 A
	EXTree* trueVal = expr->getNextChild();
	RFSMvalue* tval = checkVALUEExpr(eHandler, trueVal);

	//: ->지나치기
	expr->getNextChild();

	//(*** ? A: B)에서 B
	EXTree* falseVal = expr->getNextChild();
	RFSMvalue* fval = checkVALUEExpr(eHandler, falseVal);

	return new RFSMcondValue(boo, tval, fval, expr->getNode().getFileName(), expr->getNode().getEndLine());

}


RFSMadd* RTObjBuilder::checkADDExpr(ErrorHandler& eHandler, EXTree* expr)
{
	if(expr== NULL) return NULL;

	// 1+2+3 => 1:left, +:op, 2+3:recursive

	//left
	EXTree* left = expr->getFirstChild();
	RFSMvalue* l_val = checkVALUEExpr(eHandler, left);
	expr->delCurChild();
	//delete left;

	//operator
	EXTree* op = expr->getFirstChild();
	EXTree* opc = op->getFirstChild();
	Gtoken opNode = opc->getNode();
	expr->delCurChild();
	//delete op;

#ifdef __DEBUG_SYMBOL_
std::cout<<opNode.getData().c_str()<<"\n";
#endif

	RFSMvalue* r_val = NULL;

	if(expr->getChildrenNumber()>1){
		r_val = checkADDExpr(eHandler, expr);
	}
	else{
		EXTree* right = expr->getFirstChild();
		r_val = checkVALUEExpr(eHandler, right);
	}

	if(l_val != NULL && r_val != NULL){
		unsigned int lt = l_val->getType();
		unsigned int rt = r_val->getType();

		if(!isComputable(opNode.getType(), lt, rt)){
			///////////////////////////////////////////////
			//오류 검사 및 보고
			eHandler.addErrorCount();
			printErrMsg(TYP_UNCOMPATIBLE, &opNode);
			///////////////////////////////////////////////
		}
	}
	else{ //둘중에 하나라도 NULL이
		return NULL;
	}

	return new RFSMadd(opNode.getType(), l_val, r_val, expr->getNode().getFileName(), expr->getNode().getEndLine());
}

RFSMmulti* RTObjBuilder::checkMULExpr(ErrorHandler& eHandler, EXTree* expr)
{
	if(expr == 0) return 0;

	//left
	EXTree* left = expr->getFirstChild();
	RFSMvalue* l_val = checkVALUEExpr(eHandler, left);
	expr->delCurChild();

	//operator
	EXTree* op = expr->getFirstChild();
	EXTree* opc = op->getFirstChild();
	Gtoken opNode = opc->getNode();
	expr->delCurChild();

#ifdef __DEBUG_SYMBOL_
std::cout<<opNode.getData().c_str()<<"\n";
#endif

	//right
	RFSMvalue* r_val = NULL;
	if(expr->getChildrenNumber()>1){
		r_val = checkMULExpr(eHandler, expr);
	}
	else{
		EXTree* right = expr->getFirstChild();
		r_val = checkVALUEExpr(eHandler, right);
	}

	if(l_val != NULL && r_val != NULL){
		unsigned int lt = l_val->getType();
		unsigned int rt = r_val->getType();
		if(!isComputable(OP_MUL, lt, rt)){
			///////////////////////////////////////////
			//오류 검사 및 보고
			eHandler.addErrorCount();
			printErrMsg(TYP_UNCOMPATIBLE, &opNode);
			//////////////////////////////////////////
		}
	}

	return new RFSMmulti(opNode.getType(), l_val, r_val, expr->getNode().getFileName(), expr->getNode().getEndLine());
}

RFSMsign* RTObjBuilder::checkSIGNExpr(ErrorHandler& eHandler, EXTree* expr)
{
	EXTree* c = expr->getFirstChild();
	Gtoken sign = c->getNode();

	EXTree* c2 = expr->getNextChild();
	RFSMvalue* val = checkVALUEExpr(eHandler, c2);

	bool hasMinus = false;
	int type = sign.getType();
	if(type = OP_MIN) hasMinus = true;

	RFSMsign* rsign = new RFSMsign(hasMinus, val, sign.getFileName(), sign.getEndLine());

	return rsign;
}


/**
*	이건 if문에 있는 경우이다.(bhv, action은 리턴 타입이 없기 때문에 사용하지 못함)
*	그리고 IF문에는 action이나 bhv가 올 수 없다.
*/

RFSMcallFunc* RTObjBuilder::checkFUNCExpr(ErrorHandler& eHandler, EXTree* expr)
{
	if(expr == 0) return 0;

	EXTree* cname = expr->getFirstChild(); //CNAME
	EXTree* id = cname->getFirstChild();  //ID노드 함수명
	Gtoken nNode = id->getNode();

	std::string funcName = nNode.getData().c_str();

#ifdef __DEBUG_SYMBOL_
	std::cout<<cname->getNode().getData().c_str()<<" "<<funcName<<"\n";
#endif

	RFSMcallFunc* rfunc = NULL;
	TABfunction* tfunc = NULL;
	int result = m_symT->findFunction(funcName);

	if(result != 0) {
		eHandler.addErrorCount();
		printErrMsg(result, &nNode);
	}
	else{
		tfunc = m_symT->getFunction(funcName);
		rfunc = new RFSMcallFunc(nNode.getBlockPath(), nNode.getBlockName(), funcName, tfunc->isAction(), nNode.getFileName(), nNode.getEndLine()); 
		rfunc->setRtnType(tfunc->getRtnType());

		EXTree* params = expr->getNextChild(); //CPARAMS 노드

		if(params != NULL && params->move2FirstChild()){
			do{
				EXTree* cparam = params->getCurrentChild();	//CPARAM 노드
				checkCallParam(eHandler, &nNode, cparam, tfunc, rfunc);
			}
			while(params->move2NextChild());
		}
	}

	return rfunc;
}

/**
* @
*
*/


RFSMsym* RTObjBuilder::checkSYMBExpr(ErrorHandler& eHandler, EXTree* expr)
{
	if(expr == 0) return 0;

	EXTree* c = expr->getFirstChild();
	Gtoken sym = c->getNode();

	std::string symStr = sym.getData();

#ifdef __DEBUG_SYMBOL_
std::cout<<symStr.c_str()<<"\n";
#endif

	RFSMsym* rSym = NULL;


	//system변수 검사
	int result4 = getSysType(symStr);
	if(UNDEFINED_SYMBOL!= result4)
		return new RFSMsystem(sym.getBlockPath(), sym.getBlockName(), result4, symStr, sym.getFileName(), sym.getEndLine());

	//글로벌 검사
	int result1	= m_symT->findSymbol(symStr);//모델
	int result2	= m_symT->findEnum(symStr);//enum(사용자정의)
	//localvar검사
	int result3 = m_symT->findLocalVar(sym.getBlockPath(), symStr);


	if(result1 != 0 && result2 != 0 && result3 != 0){
		eHandler.addErrorCount();
		printErrMsg(SYM_SYMBOL_ENUM_LOCALVAR_NOT_FOUND, &sym);
	}
	else{
		//지역변수가 글로벌 보다 우선 (하지만 글로벌은 (모델명.이름)이기 때문에 이름이 겹치기 힘들다.. 다만 behavior내부에 모델정의를 허용하면 중요해진다.
		if(result3==0){
			TABsymbol* tvar	= m_symT->getLocalVar(sym.getBlockPath(), sym.getData());
			rSym = new RFSMvariable(tvar->getBlockPath(), tvar->getBlockName(), tvar->getName(), tvar->getSymbolType(), G_LOCAL, sym.getFileName(), sym.getEndLine());
		}
		else if(result1 == 0){
			TABsymbol* tSym	= m_symT->getSymbol(symStr);
			unsigned int stype = tSym->getSymbolMod(); //G_IN, G_OUT, G_GVAR, G_WVAR
			rSym = new RFSMvariable(tSym->getBlockPath(), tSym->getBlockName(), tSym->getName(),tSym->getSymbolType(), stype, sym.getFileName(), sym.getEndLine());
		}		 
		else if(result2 == 0)
		{
			TABenum* tenu	= m_symT->getEnum(symStr);
			rSym = new RFSMenum(tenu->getValue(), TYP_ENUM, tenu->getTypeName(), tenu->getName(), sym.getFileName(), sym.getEndLine());
		}
	}

	return rSym;
}

RFSMliteral* RTObjBuilder::checkLITEExpr(ErrorHandler& eHandler, EXTree* expr)
{
	if(expr == 0) return 0;

	EXTree* c = expr->getFirstChild();
	Gtoken lite = c->getNode();

	std::string liteStr = lite.getData();
	unsigned int type = lite.getType();

#ifdef __DEBUG_SYMBOL_
std::cout<<liteStr.c_str()<<"\n";
#endif

return new RFSMliteral(lite.getBlockPath(), lite.getBlockName(), liteStr, type, expr->getNode().getFileName(), expr->getNode().getEndLine());
}


SymTAB4Task* RTObjBuilder::getSymT()
{
	return m_symT;
}

