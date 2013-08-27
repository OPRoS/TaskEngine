/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/GstmtExpr.h"

GstmtExpr::GstmtExpr(std::string file, int eol, std::string blockname, std::string blockpath):Gstmt(STMT_expr, file, eol, blockname, blockpath)
{
	m_exprTree = NULL;
}

GstmtExpr:: ~GstmtExpr(void)
{
	clear();
}

void GstmtExpr::clear()
{
	//m_exprTree->delAll();	
	delete m_exprTree;
}

void GstmtExpr::setExprTree(EXTree* exprTree)
{
	if(m_exprTree != NULL) clear();
	m_exprTree = exprTree;
}

EXTree* GstmtExpr::getExprTree()
{
	return m_exprTree;
}

void GstmtExpr::print()
{	
	m_exprTree->print();
}

void GstmtExpr::print(unsigned int indent)
{
	titlePrint(indent, std::string("EXPR"));

	if(m_exprTree != NULL){
		//Tree의 root에는 의미 없는 데이터이기 때문에 생략
		EXTree* expr = m_exprTree->getFirstChild();
		expr->print(indent+1);
	}
}
