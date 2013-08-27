/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/symtab/SymTAB4Task.h"

SymTAB4Task::SymTAB4Task(void)
{
	USER_TYPE_CNT = 10000;
	m_refCount = 0;

	m_modelNameList	= NULL;
	m_symbolList	= NULL;
	m_enumList		= NULL;
	m_actionList	= NULL;
	m_behaviorList	= NULL;
	m_connectorList	= NULL;
	m_stateList		= NULL;
	m_funcList		= NULL;
	m_localVarList	= NULL;
}

SymTAB4Task:: ~SymTAB4Task(void)
{
	if(m_modelNameList != NULL){
		while(m_modelNameList->getSize()>0 && m_modelNameList->moveHead()){
			m_modelNameList->delHead();
		}
		delete m_modelNameList;
		m_modelNameList= NULL;
	}

	if(m_symbolList != NULL){
		while(m_symbolList->getSize()>0 && m_symbolList->moveHead()){
			TABsymbol* obj = m_symbolList->getCurObject();
			delete obj;
			m_symbolList->delHead();
		}
		delete m_symbolList;
		m_symbolList= NULL;
	}

	if(m_enumList != NULL){
		while(m_enumList->getSize()>0 && m_enumList->moveHead()){
			TABenum* obj = m_enumList->getCurObject();
			delete obj;
			m_enumList->delHead();
		}
		delete m_enumList;
		m_enumList= NULL;
	}

	if(m_actionList != NULL){
		while(m_actionList->getSize()>0 && m_actionList->moveHead()){
			TABaction* obj = m_actionList->getCurObject();
			delete obj;
			m_actionList->delHead();
		}
		delete m_actionList;
		m_actionList= NULL;
	}

	if(m_funcList != NULL){
		while(m_funcList->getSize()>0 && m_funcList->moveHead()){
			TABfunction* obj = m_funcList->getCurObject();
			delete obj;
			m_funcList->delHead();
		}
		delete m_funcList;
		m_funcList= NULL;
	}

	if(m_behaviorList != NULL){
		while(m_behaviorList->getSize()>0 && m_behaviorList->moveHead()){
			TABbehavior* obj = m_behaviorList->getCurObject();
			delete obj;
			m_behaviorList->delHead();
		}
		delete m_behaviorList;
		m_behaviorList= NULL;
	}

	if(m_connectorList != NULL){
		while(m_connectorList->getSize()>0 && m_connectorList->moveHead()){
			TABconnector* obj = m_connectorList->getCurObject();
			delete obj;
			m_connectorList->delHead();
		}
		delete m_connectorList;
		m_connectorList= NULL;
	}

	if(m_stateList != NULL){
		while(m_stateList->getSize()>0 && m_stateList->moveHead()){
			TABstate* obj = m_stateList->getCurObject();
			delete obj;
			m_stateList->delHead();
		}
		delete m_stateList;
		m_stateList= NULL;
	}

	if(m_localVarList != NULL){
		while(m_localVarList->getSize()>0 && m_localVarList->moveHead()){
			TABsymbol* obj = m_localVarList->getCurObject();
			delete obj;
			m_localVarList->delHead();
		}
		delete m_localVarList;
		m_localVarList= NULL;
	}

}

void SymTAB4Task::pluRefCount()
{
	m_refCount++;
}


void SymTAB4Task::minRefCount()
{
	m_refCount--;
}

int SymTAB4Task::getWorkerRefCount()
{
	return m_refCount;
}

/*
int SymTAB4Task::addWorkerName(std::string wname)
{
	std::vector<std::string>::iterator it = m_workerNames.begin();
	std::vector<std::string>::iterator end = m_workerNames.end();

	for(; it<end;it++){
		if(it->compare(wname)==0)
			return 0;
	}

	m_workerNames.push_back(wname);

	return 0;
}
*/


int SymTAB4Task::addModel(std::string model)
{
	int result = checkMoldelDuplication(model);
	if(result == SYM_NOT_DUPLICATED){
		if(m_modelNameList == 0)
			m_modelNameList = new EList<std::string>();
		m_modelNameList->addTail(model);
		return 0;
	}
	return result;
}


int SymTAB4Task::addSymbol(TABsymbol* symbol)
{
	int result = checkMoldelDuplication(symbol->getName());
	if(result == SYM_NOT_DUPLICATED){
		if(m_symbolList == 0)
			m_symbolList = new EList<TABsymbol*>();
		m_symbolList->addTail(symbol);
		return 0;
	}
	return result;
}


int SymTAB4Task::addLocalVar(std::string blockpath, TABsymbol* symbol)
{
	int result1 = checkMoldelDuplication(symbol->getName());

	if(result1 != SYM_NOT_DUPLICATED)//중복된 경우
		return result1;

	/* //result의 종류
	if(result1 == SYM_SYSVAR_DUPLICATED)
		return result1;
	else if(result1 == SYM_ENUM_DUPLICATED)
		return result1;
	else if(result1 == SYM_MODEL_DUPLICATED)
		return result1;
	else if(result1 == SYM_SYMBOL_DUPLICATED)
		return result1;
	else if(result1 == SYM_STATE_DUPLICATED)
		return result1;
	*/


	else{//중복이 아니면
		std::string vname = blockpath;
		vname.append(".").append(symbol->getName());
		int result2 = findSameLocalVar(vname);
		if(result2 == SYM_LOCAL_VAR_NOT_FOUND){
			if(m_localVarList == 0)
				m_localVarList = new EList<TABsymbol*>();
			m_localVarList->addTail(symbol);
			return 0;
		}
		return SYM_LOCAL_VAR_DUPLICATED;
	}
	return result1;
}

int SymTAB4Task::addEnum(TABenum* enu)
{
	int result = checkMoldelDuplication(enu->getName());

	if(result == SYM_NOT_DUPLICATED){

		//먼저 타입 식별자를 부여한다.
		int r = findEnumType(enu->getTypeName());
		if(r == SYM_ENUM_NOT_DEFIENED)
			enu->setType(USER_TYPE_CNT++);
		else
			enu->setType(getUserTypeID(enu->getTypeName()));

		if(m_enumList == 0)
			m_enumList = new EList<TABenum*>();
		m_enumList->addTail(enu);
		return 0;
	}
	return result;
}

int SymTAB4Task::addAction(TABaction* action)
{
	int result = checkMoldelDuplication(action->getName());
	if(result == SYM_NOT_DUPLICATED){
		if(m_actionList == 0)
			m_actionList = new EList<TABaction*>();
		m_actionList->addTail(action);
		return 0;
	}
	return result;
}

int SymTAB4Task::addFunction(TABfunction* func)
{
	int result = checkMoldelDuplication(func->getName());
	if(result == SYM_NOT_DUPLICATED){
		if(m_funcList == 0)
			m_funcList = new EList<TABfunction*>();
		m_funcList->addTail(func);
		return 0;
	}
	return result;
}

int SymTAB4Task::addBehavior(TABbehavior* bhv)
{
	int result = checkMoldelDuplication(bhv->getName());
	if(result == SYM_NOT_DUPLICATED){
		if(m_behaviorList == 0)
			m_behaviorList = new EList<TABbehavior*>();
		m_behaviorList->addTail(bhv);
		return 0;
	}
	return result;
}

int SymTAB4Task::addConnector(TABconnector* con)
{
	int result = checkMoldelDuplication(con->getName());
	if(result == SYM_NOT_DUPLICATED){
		if(m_connectorList == 0)
			m_connectorList = new EList<TABconnector*>();
		m_connectorList->addTail(con);
		return 0;
	}
	return result;
}


int SymTAB4Task::addState(TABstate* state)
{
	int result = checkMoldelDuplication(state->getName());
	if(result == SYM_NOT_DUPLICATED){
		if(m_stateList == 0)
			m_stateList = new EList<TABstate*>();
		m_stateList->addTail(state);
		return 0;
	}
	return result;
}

/*
* 테이블에 저장하기 전에 중복을 검사하는 함수
다른곳에서 호출되지 않는다.
*/
int SymTAB4Task::checkMoldelDuplication(std::string name)
{
	/*model명과 같은 것이 있으면 안된다.*/
	//action, task와 이들의 파라메터 명, enum명 state명

	//task명, action명이 서로 중복되면 안된다.

	int result = getSysType(name);
	if(UNDEFINED_SYMBOL!= result)
		return SYM_SYSVAR_DUPLICATED;

	if(findEnum(name) == 0)
		return SYM_ENUM_DUPLICATED;
	if(findModel(name) == 0)
		return SYM_MODEL_DUPLICATED;
	if(findSymbol(name) == 0)
		return SYM_SYMBOL_DUPLICATED;
	if(findAction(name) == 0)
		return SYM_ACTION_DUPLICATED;
	if(findFunction(name) == 0)
		return SYM_FUNC_DUPLICATED;
	if(findBehavior(name) == 0)
		return SYM_TASK_DUPLICATED;
	if(findConnector(name)==0)
		return SYM_CONN_DUPLICATED;
	if(findState(name) == 0)
		return SYM_STATE_DUPLICATED;


	return SYM_NOT_DUPLICATED;
}

int SymTAB4Task::findSymbol(std::string name)
{
	if(m_symbolList != 0 && m_symbolList->getSize()>0){
		if(m_symbolList->moveHead()){
			do{
				TABsymbol *sym = m_symbolList->getCurObject();
				if(sym->isSameName(name))
					return 0;
			}
			while(m_symbolList->moveNext());
		}
	}
	return SYM_SYMBOL_NOT_FOUND;
}



/**
*3.a를 찾고 없으면 1.2.a를 찾고 없으면 1.a를 찾는다.
*
*@blockpath
   전형적인 형태는 1.2.3
*@name
   전형적인 형태는 m_var
*/
int SymTAB4Task::findLocalVar(std::string blockpath, std::string name)
{
	//이름이 0.1.2.abc 인데 같은 이름이 없다면
	//0.1.abc, 0.abc등도 검사해야 한다.

	if(m_localVarList != 0 &&m_localVarList->getSize()>0){
		std::vector<std::string> path = Estring::split(blockpath, ".");

		for(unsigned int k=path.size(); k>0; k--){
			std::string c_path="";
			for(unsigned int i=0; i<k; i++){
				c_path.append(path.at(i));
				if(i+1<k)c_path.append(".");
			}

			if(m_localVarList->moveHead()){
				do{
					TABsymbol *sym = m_localVarList->getCurObject();
					if(sym->isSameName(name) && sym->getBlockPath().compare(c_path)==0)
						return 0;
				}
				while(m_localVarList->moveNext());
			}
		}
	}
	return SYM_LOCAL_VAR_NOT_FOUND;
}

int SymTAB4Task::findSameLocalVar(std::string name)
{
	//이름이 0.1.2.abc 인데 같은 이름이 없다면
	//0.1.abc, 0.abc등도 검사해야 한다.

	if(m_localVarList != 0 && m_localVarList->getSize()>0 && m_localVarList->moveHead()){				
		do{
			TABsymbol *sym  = m_localVarList->getCurObject();
			std::string vname = sym->getBlockPath();
			vname.append(".").append(sym->getName());
			if(vname.compare(name)==0)
				return 0;
		}
		while(m_localVarList->moveNext());				
	}
	
	return SYM_LOCAL_VAR_NOT_FOUND;
}


int SymTAB4Task::findModel(std::string name)
{
	if(m_modelNameList != 0 && m_modelNameList->getSize()>0){
		if(m_modelNameList->moveHead()){
			do{
				std::string m = m_modelNameList->getCurObject();
				if(m.compare(name)==0)
					return 0;
			}
			while(m_modelNameList->moveNext());
		}
	}
	return SYM_MODEL_NOT_FOUND;
}

int SymTAB4Task::findEnum(std::string name)
{
	if(m_enumList != 0 && m_enumList->getSize()>0){
		if(m_enumList->moveHead()){
			do{
				TABenum *enu = m_enumList->getCurObject();
				if(enu->isSameName(name))
					return 0;
			}
			while(m_enumList->moveNext());
		}
	}
	return SYM_ENUM_NOT_FOUND;
}

int SymTAB4Task::findEnumType(std::string name)
{
	if(m_enumList != 0 && m_enumList->getSize()>0){
		if(m_enumList->moveHead()){
			do{
				TABenum *enu = m_enumList->getCurObject();
				std::string eTypeName = enu->getTypeName();
				if(eTypeName.compare(name)==0)
					return 0;
			}
			while(m_enumList->moveNext());
		}
	}
	return SYM_ENUM_NOT_DEFIENED;
}

int SymTAB4Task::findAction(std::string name)
{
	if(m_actionList != 0 && m_actionList->getSize()>0){
		if(m_actionList->moveHead()){
			do{
				TABaction *action = m_actionList->getCurObject();
				std::string aName = action->getName();
				if(aName.compare(name)==0)
					return 0;
			}
			while(m_actionList->moveNext());
		}
	}
	return SYM_ACTION_NOT_FOUND;
}

int SymTAB4Task::findFunction(std::string name)
{
	if(m_funcList != 0 && m_funcList->getSize()>0){
		if(m_funcList->moveHead()){
			do{
				TABfunction *func = m_funcList->getCurObject();
				std::string fName = func->getName();
				if(fName.compare(name)==0)
					return 0;
			}
			while(m_funcList->moveNext());
		}
	}
	return SYM_FUNC_NOT_FOUND;
}

int SymTAB4Task::findBehavior(std::string name)
{
	if(m_behaviorList != 0 && m_behaviorList->getSize()>0){
		if(m_behaviorList->moveHead()){
			do{
				TABbehavior *task = m_behaviorList->getCurObject();
				std::string tName = task->getName();
				if(tName.compare(name)==0)
					return 0;
			}
			while(m_behaviorList->moveNext());
		}
	}
	return SYM_TASK_NOT_FOUND;
}

int SymTAB4Task::findConnector(std::string name)
{
	if(m_connectorList != 0 && m_connectorList->getSize()>0){
		if(m_connectorList->moveHead()){
			do{
				TABconnector *con = m_connectorList->getCurObject();
				std::string cName = con->getName();
				if(cName.compare(name)==0)
					return 0;
			}
			while(m_connectorList->moveNext());
		}
	}
	return SYM_CONN_NOT_FOUND;
}

/**
* 주의: state이름은 task명.state명 형식으로 저장되어 있다.
* Gstate에서부터 그렇게 만들어 진다.
*/
int SymTAB4Task::findState(std::string name)
{
	if(m_stateList != 0 && m_stateList->getSize()>0){
		if(m_stateList->moveHead()){
			do{
				TABstate *state = m_stateList->getCurObject();
				std::string sName = state->getName();
				if(sName.compare(name)==0)
					return 0;
			}
			while(m_stateList->moveNext());
		}
	}
	return SYM_STATE_NOT_FOUND;
}

TABsymbol* SymTAB4Task::getSymbol(std::string name)
{
	if(m_symbolList != 0 && m_symbolList->getSize()>0 && m_symbolList->moveHead()){
		do{
			TABsymbol *sym = m_symbolList->getCurObject();
			if(sym->isSameName(name))
				return sym;
		}
		while(m_symbolList->moveNext());
	}

	return 0;
}

//////////////////////////////////////////////////////////////////
TABsymbol* SymTAB4Task::getLocalVar(std::string blockpath, std::string name)
{
	//이름이 0.1.2.abc 인데 같은 이름이 없다면
	//0.1.abc, 0.abc등도 검사해야 한다.

	std::vector<std::string> path = Estring::split(blockpath, ".");

	if(m_localVarList != 0 &&m_localVarList->getSize()>0){
		for(unsigned int k=path.size(); k>0; k--){
			std::string c_path="";
			for(unsigned int i=0; i<k; i++){
				c_path.append(path.at(i));
				if(i+1<k)c_path.append(".");
			}

			if(m_localVarList->moveHead()){
				do{
					TABsymbol *sym = m_localVarList->getCurObject();
					if(sym->isSameName(name) && sym->getBlockPath().compare(c_path)==0)
						return sym;
				}
				while(m_localVarList->moveNext());
			}
		}
	}
	return 0;
}

TABenum* SymTAB4Task::getEnum(std::string name)
{
	if(m_enumList != 0 && m_enumList->getSize()>0){
		if(m_enumList->moveHead()){
			do{
				TABenum *enu = m_enumList->getCurObject();
				std::string eName = enu->getName();
				if(eName.compare(name)==0)
					return enu;
			}
			while(m_enumList->moveNext());
		}
	}

	return 0;
}

TABaction* SymTAB4Task::getAction(std::string name)
{
	if(m_actionList != 0 && m_actionList->getSize()>0){
		if(m_actionList->moveHead()){
			do{
				TABaction *action = m_actionList->getCurObject();
				std::string aName = action->getName();
				if(aName.compare(name)==0)
					return action;
			}
			while(m_actionList->moveNext());
		}
	}

	return 0;
}

TABfunction* SymTAB4Task::getFunction(std::string name)
{
	if(m_funcList != 0 && m_funcList->getSize()>0){
		if(m_funcList->moveHead()){
			do{
				TABfunction *func = m_funcList->getCurObject();
				std::string fName = func->getName();
				if(fName.compare(name)==0)
					return func;
			}
			while(m_funcList->moveNext());
		}
	}

	return 0;
}

TABbehavior* SymTAB4Task::getBehavior(std::string name)
{
	if(m_behaviorList != 0 && m_behaviorList->getSize()>0){
		if(m_behaviorList->moveHead()){
			do{
				TABbehavior *task = m_behaviorList->getCurObject();
				std::string tName = task->getName();
				if(tName.compare(name)==0)
					return task;
			}
			while(m_behaviorList->moveNext());
		}
	}
	return 0;
}

TABconnector* SymTAB4Task::getConnector(std::string name)
{
	if(m_connectorList != 0 && m_connectorList->getSize()>0){
		if(m_connectorList->moveHead()){
			do{
				TABconnector *con = m_connectorList->getCurObject();
				std::string cName = con->getName();
				if(cName.compare(name)==0)
					return con;
			}
			while(m_connectorList->moveNext());
		}
	}
	return 0;
}

TABstate* SymTAB4Task::getState(std::string name)
{
	if(m_stateList != 0 && m_stateList->getSize()>0){
		if(m_stateList->moveHead()){
			do{
				TABstate *state = m_stateList->getCurObject();
				std::string sName = state->getName();
				if(sName.compare(name)==0)
					return state;
			}
			while(m_stateList->moveNext());
		}
	}

	return 0;
}


int SymTAB4Task::getUserTypeID(std::string typeName)
{
	if(m_enumList != 0 && m_enumList->getSize()>0){
		if(m_enumList->moveHead()){
			do{
				TABenum *enu = m_enumList->getCurObject();
				if(enu->getTypeName().compare(typeName)==0)
					return enu->getType();
			}
			while(m_enumList->moveNext());
		}
	}
	return UNDEFINED_TYPE;
}

/**
입력 Model내에 정의된 모든 Symbol들을 찾는다.
*/
std::vector<TABsymbol*> SymTAB4Task::getSymbols(std::string modelname)
{
	std::vector<TABsymbol*> syms;
	//std::vector<TABsymbol*>::iterator it = syms.begin();

	if(m_symbolList != 0 && m_symbolList->getSize()>0 && m_symbolList->moveHead()){
		do{
			TABsymbol *sym = m_symbolList->getCurObject();
			if(sym->getName().find(modelname.c_str())==0){
				syms.push_back(sym);
			}			
		}
		while(m_symbolList->moveNext());
	}

	return syms;
}

/**
입력 Model내에 정의된 모든 Function들을 찾는다.
*/
std::vector<TABfunction*> SymTAB4Task::getFunctions(std::string modelname)
{
	std::vector<TABfunction*> funcs;
	//std::vector<TABfunction*>::iterator it;

	if(m_funcList != 0 && m_funcList->getSize()>0){
		if(m_funcList->moveHead()){
			do{
				TABfunction *func = m_funcList->getCurObject();
				std::string fName = func->getName();
				if(fName.find(modelname.c_str())==0){
					funcs.push_back(func);
				}							
			}
			while(m_funcList->moveNext());
		}
	}

	return funcs;
}

EList<std::string>*	SymTAB4Task::getModels()
{
	return m_modelNameList;
}

/*
int SymTAB4Task::actionParamComp(std::string name, EList<Gparameter*>* params)
{
	return -1;
}
int SymTAB4Task::taskParamComp(std::string name, EList<Gparameter*>* params)
{
	return -1;
}
*/

void SymTAB4Task::printEnums()
{
	printf("----------------------------------< Enum Table >-----------------------------\n");
	printf("%6s | %15s | %34s | %7s\n", "TypeID", "Type", "Name","Value");
	printf("-----------------------------------------------------------------------------\n");
	if(m_enumList != 0 && m_enumList->getSize()>0){
		if(m_enumList->moveHead()){
			do{
				TABenum *enu = m_enumList->getCurObject();
				printf("%6d	 | ", enu->getType());
				printf("%15s | ", enu->getTypeName().c_str());
				printf("%34s | ", enu->getName().c_str());
				printf("%7d\n", enu->getValue());
			}
			while(m_enumList->moveNext());
		}
	}
	printf("----------------------------------------------------------------------------\n");
}

void SymTAB4Task::printModels()
{
	printf("---------------------------< Model Symbol Table >----------------------------\n");
	printf("%15s | %25s | %3s | %3s | %7s\n", "Block_path","Name","TYP","MOD","Value");
	printf("-----------------------------------------------------------------------------\n");
	if(m_symbolList != 0 && m_symbolList->getSize()>0){
		if(m_symbolList->moveHead()){
			do{
				TABsymbol *sym = m_symbolList->getCurObject();

				printf("%15s | ", sym->getBlockPath().c_str());
				printf("%25s | ", sym->getName().c_str());
				printf("%3d | ", sym->getSymbolType());
				printf("%3d | ", sym->getSymbolMod());
				printf("%7s\n", sym->getValueStr());
			}
			while(m_symbolList->moveNext());
		}
	}
	printf("-----------------------------------------------------------------------------\n");
}

void SymTAB4Task::printLocalVariables()
{
	printf("---------------------------< Local Variable Table >--------------------------\n");
	printf("%15s | %25s | %3s\n", "Block_path","Name","TYP");
	printf("-----------------------------------------------------------------------------\n");
	if(m_localVarList != 0 && m_localVarList->getSize()>0){
		if(m_localVarList->moveHead()){
			do{
				TABsymbol *sym = m_localVarList->getCurObject();

				printf("%15s | ", sym->getBlockPath().c_str());
				printf("%25s | ", sym->getName().c_str());
				printf("%3d\n", sym->getSymbolType());
			}
			while(m_localVarList->moveNext());
		}
	}
	printf("-----------------------------------------------------------------------------\n");
}

void SymTAB4Task::printActions()
{
	printf("------------------------------< Action Table >-------------------------------\n");
	printf("%10s | %35s\n", "Name", "Parameters");
	printf("-----------------------------------------------------------------------------\n");
	if(m_actionList != 0 && m_actionList->getSize()>0){
		if(m_actionList->moveHead()){
			do{
				TABaction* action = m_actionList->getCurObject();
				printf("%10s | ", action->getName().c_str());

				EList<TABparameter*>* params = action->getParameters();
				if(params!=0 && params->getSize()>0){
					do{
						TABparameter *param = params->getCurObject();
						printf("(%s, %s)",param->getType().c_str(), param->getName().c_str());

						//param->print(0);
					}
					while(params->moveNext());
					printf("\n");
				}
				else{
					printf("%35s\n", "no parameters");
				}
			}
			while(m_actionList->moveNext());
		}
	}
	printf("-----------------------------------------------------------------------------\n");
}

void SymTAB4Task::printFunctions()
{
	printf("---------------------------< Function Table >--------------------------------\n");
	printf("%5s | %15s | %35s\n", "Rtn", "Name", "Parameters");
	printf("-----------------------------------------------------------------------------\n");
	if(m_funcList != 0 && m_funcList->getSize()>0){
		if(m_funcList->moveHead()){
			do{

				TABfunction* func = m_funcList->getCurObject();
				printf("%5d | ", func->getRtnType());
				printf("%15s | ", func->getName().c_str());

				EList<TABparameter*>* params = func->getParameters();
				if(params!=0 && params->getSize()>0){
					do{
						TABparameter *param = params->getCurObject();
						printf("(%s, %s)", param->getType().c_str(), param->getName().c_str());

						//param->print(0);
					}
					while(params->moveNext());
					printf("\n");
				}
				else{
					printf("%35s\n", "no parameters");
				}
			}
			while(m_funcList->moveNext());
		}
	}
	printf("-----------------------------------------------------------------------------\n");
}

void SymTAB4Task::printStates()
{
	printf("-----------------------------< State Table >---------------------------------\n");
	printf("%10s | %35s\n", "Task", "State");
	printf("-----------------------------------------------------------------------------\n");
	if(m_stateList != 0 && m_stateList->getSize()>0){
		if(m_stateList->moveHead()){
			do{
				TABstate* state = m_stateList->getCurObject();
				std::string taskName = state->getTaskName();
				std::string stateName = state->getName();
				printf("%10s | %35s\n", taskName.c_str(), stateName.c_str());
			}
			while(m_stateList->moveNext());
		}
	}
	printf("-----------------------------------------------------------------------------\n");
}
