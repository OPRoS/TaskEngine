/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "common/Gdatatype.h"
//#include "EList.h"
#include "runtime/symtab/TABsymbol.h"
#include "runtime/symtab/TABenum.h"
#include "runtime/symtab/TABaction.h"
#include "runtime/symtab/TABfunction.h"
#include "runtime/symtab/TABbehavior.h"
#include "runtime/symtab/TABconnector.h"
#include "runtime/symtab/TABstate.h"
#include <vector>
#include "ds/Estring.h"

class SymTAB4Task
{
private:
	int	USER_TYPE_CNT; // ����� Ÿ�Կ� ���� �ĺ���
	int m_refCount;    // �� �ɺ� ���̺��� �����ϴ� worker�� ��,

	EList<std::string>*		m_modelNameList;	
	EList<TABenum*>*		m_enumList;
	EList<TABaction*>*		m_actionList;  //GparsedResult�� �� ���̾��µ� ���� �δ� ������ ���߿� namespace�� �߰� �� �� ���Ƽ� �̴�. a.b.c
	EList<TABfunction*>*	m_funcList;
	EList<TABstate*>*		m_stateList;
	EList<TABbehavior*>*	m_behaviorList;
	EList<TABconnector*>*	m_connectorList;

	//�Ʒ� �κκ��� ����Ǵ� task�ν��Ͻ��� �������̴�.
	EList<TABsymbol*>*      m_localVarList;  //wvar
	EList<TABsymbol*>*		m_symbolList;    //���ú�����

public:
	SymTAB4Task(void);
	virtual ~SymTAB4Task(void);

	int getWorkerRefCount(); //�� ���� �˻��ϰ� 1�����̸� �ܺο��� SymTAB4Task�� ���� �Ѵ�.
	void pluRefCount();
	void minRefCount();

	int addModel(std::string);
	int addSymbol(TABsymbol*);
	int addEnum(TABenum*);
	int addAction(TABaction*);
	int addFunction(TABfunction*);
	int addState(TABstate* state);
	int addBehavior(TABbehavior* task);
	int addConnector(TABconnector* con);
	int addLocalVar(std::string blockpath,TABsymbol*);
	int checkMoldelDuplication(std::string);

	unsigned int	getEnumValue(std::string symName);
	int				getIntValue(std::string symName);
	bool			getBoolValue(std::string symName);
	float			getFloatValue(std::string symName);

	//�˻�
	int findSymbol(std::string);
	int findModel(std::string name);
	int findEnum(std::string);
	int findEnumType(std::string name);
	int findAction(std::string);
	int findFunction(std::string);
	int findState(std::string);
	int findBehavior(std::string);
	int findConnector(std::string);
	int findLocalVar(std::string blockpath, std::string);
	int findSameLocalVar(std::string name);

	std::vector<TABsymbol*>		getSymbols(std::string);
	std::vector<TABfunction*>	getFunctions(std::string);
	EList<std::string>*			getModels();

	TABsymbol*		getSymbol(std::string);
	TABenum*		getEnum(std::string);
	TABaction*		getAction(std::string);
	TABfunction*	getFunction(std::string);
	TABstate*		getState(std::string);
	TABbehavior*	getBehavior(std::string name);
	TABconnector*	getConnector(std::string);
	TABsymbol*		getLocalVar(std::string blockpath,std::string);
	int				getUserTypeID(std::string);

	//int actionParamComp(std::string, EList<Gparameter*>*);
	//int taskParamComp(std::string, EList<Gparameter*>*);

	void printEnums();
	void printModels();
	void printActions();
	void printStates();
	void printFunctions();
	void printLocalVariables();
};
