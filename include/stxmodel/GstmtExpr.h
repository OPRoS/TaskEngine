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
#include "stxmodel/Gstmt.h"
#include "stxmodel/Ggram.h"
#include "stxmodel/Gtoken.h"
#include "common/EXTree.h"

class GstmtExpr : public Gstmt
{

private:
	std::string		type;
	EXTree*	m_exprTree;

public:
	GstmtExpr(std::string file, int eol, std::string blockname, std::string blockpath);
	virtual ~GstmtExpr(void);

	void setExprTree(EXTree*);
	void print();
	void clear();
	EXTree* getExprTree(); 

	
	char* toString(){return NULL;};
	void print(unsigned int);
};
