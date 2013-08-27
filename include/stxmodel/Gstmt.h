/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include <iostream>
#include "Ggram.h"
#include <string>

class Gstmt : public Ggram
{
private:
	std::string	m_stmtName;

public:
	Gstmt(std::string, std::string, int, std::string, std::string);
	Gstmt(std::string, std::string);
	virtual ~Gstmt(void);	

	std::string	getStmtName();
};

const std::string STMT_VarDecl		= std::string("Variable declaration");
//static std::string STMT_Asmt 		= std::string("Assignment");
//static std::string STMT_CallAction= std::string("Action-call");
const std::string STMT_CallFunc 	= std::string("Function-call");
const std::string STMT_CallBhv		= std::string("Expand-behavior");
//static std::string STMT_CallCon	= std::string("Expand-connector");
const std::string STMT_goto			= std::string("Goto");
const std::string STMT_if			= std::string("If");
//static std::string STMT_conexer	= std::string("Conexer"); //block 내부가 일반적으로 실행
//static std::string STMT_par		= std::string("Parallel");
//static std::string STMT_seq		= std::string("Sequencial");
const std::string STMT_with			= std::string("With");
const std::string STMT_SLEEP		= std::string("SLEEP");
const std::string STMT_SYNCH		= std::string("SYNCH");
const std::string STMT_Block		= std::string("Block"); //block 내부가 일반적으로 실행

//아래는 stmt가 아닌데.. 
const std::string STMT_cond			= std::string("Condition expression");
const std::string STMT_expr			= std::string("Expression");
