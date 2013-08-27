/************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "common/antlrIncl.h"
#include "stxmodel/GstmtExpr.h"
//#include "stxmodel/GstmtBlock.h"
#include "stxmodel/Gtoken.h"
#include "stxmodel/GstmtIfDo.h"
#include "stxmodel/GstmtIfElse.h"
#include "stxmodel/Gcondition.h"
//#include "ds/EList.h"
//#include "ds/ETree.h"

class GstmtIf : public Gstmt
{
private:
	Gcondition*		m_cond;			//if조건
	Gstmt*			m_ifAction;		//조건 ture일 때 실행
	Gstmt*			m_elseAction;

	//bool iterDNF(ETree<Gtoken>* parent, ETree<Gtoken>* child);

public:
	GstmtIf(std::string file, int eol, std::string blockname, std::string blockpath);
	virtual ~GstmtIf(void);

	void setIfCondition(Gcondition*);
	void setIfAction(Gstmt*);
	void setElseAction(Gstmt*);
	//void toDNF();

	Gcondition*		getIfCond();
	Gstmt*			getIfAction();
	Gstmt*			getElse();

	char* toString(){return NULL;};
	void print(unsigned int);
};
