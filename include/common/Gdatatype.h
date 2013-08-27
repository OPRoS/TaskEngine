/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

//#include <iostream>
#include <sstream>
#include <string>
//#include <stdexcept>



typedef unsigned int usint;

#define G_ENUM		1
#define G_MODEL		2

#define G_PRIM		3
#define G_SYMB		4

#define G_VARIN		6
#define G_VAROUT	7
#define G_GVAR		8
#define G_WVAR		9
#define G_LOCAL		10

//#define SYM_IN		1 //위와 중복 하나로 통일
//#define SYM_OUT		2
#define SYM_LOCAL	3

#define MODEL_FILE  1
#define TASK_FILE	2
#define WORKER_FILE 3

#define TYP_VOID			0
#define TYP_BOOL			1
#define TYP_CHAR			2
#define TYP_STRING			3
#define TYP_BYTE			4
#define TYP_SHORT			5
#define TYP_INT				6
#define TYP_LONG			7
#define TYP_FLOAT			8
#define TYP_DOUBLE			9

#define TYP_ENUM				19
#define TYP_NOT_PRIMITIVE		20
#define TYP_UNCOMPATIBLE		21
#define TYP_RVALUE_IS_BIGGER	22

#define TYP_SYS_STATETIME		23
#define TYP_SYS_TASKTIME		24
#define TYP_SYS_BEHAVIORTIME	25


#define INITIAL_STATE		1
#define GOAL_STATE			2

#define AND_CONNECTOR		1
#define OR_CONNECTOR		2
#define XOR_CONNECTOR		3

#define SYNCHRO				1
#define ASYNCHRO			2
#define NOTHREAD			3

#define CONEXER				1
#define SEQEXER				2

#define UNDEFINED_TYPE		-1
#define UNDEFINED_SYMBOL    1000



//SYM 테이블 관련 error -> 0이 아닌수이어야 한다.
#define SYM_NOT_DUPLICATED          001

#define SYM_ENUM_NOT_FOUND			201
#define SYM_ENUM_DUPLICATED			203  //enum이 없다. e.g) BallPrecision.palyer
#define SYM_ENUM_NOT_DEFIENED		204 //enum 타입 자체가 없다.

#define SYM_MODEL_DUPLICATED		301
#define SYM_MODEL_NOT_FOUND			302
#define SYM_MODEL_CANNOT_BE_TYPE	303
#define SYM_MODEL_NOT_DEFIENED		304

#define SYM_SYMBOL_ENUM_LOCALVAR_NOT_FOUND 400
#define SYM_SYMBOL_DUPLICATED		401
#define SYM_SYMBOL_NOT_FOUND		402

#define SYM_ACTION_NOT_FOUND		501
#define SYM_ACTION_DUPLICATED		502

#define SYM_FUNC_NOT_FOUND			603
#define SYM_FUNC_DUPLICATED			602
#define SYM_FUNC_PARAM_NAME_WRONG   604
#define SYM_FUNC_PARAM_TYPE_WRONG   605
#define SYM_FUNC_PARAM_FORM_WRONG   606

#define LOCALVAR_NAME_DUPLICATED    608

#define SYM_TASK_NOT_FOUND			701
#define SYM_TASK_DUPLICATED			702

#define SYM_STATE_NOT_FOUND			705
#define SYM_STATE_DUPLICATED		706

#define SYM_CONN_NOT_FOUND			707
#define SYM_CONN_DUPLICATED			708

#define SYM_BEHA_N_CONN_NOT_FOUND	709 

#define SYM_TYPE_MISS_MATCHED       800

#define SYM_LVALUE_MUSTBE_VAR		801

#define SYM_LOCAL_VAR_DUPLICATED    802
#define SYM_LOCAL_VAR_NOT_FOUND		803

#define SYM_SYSVAR_DUPLICATED       804

#define CONNECTOR_TYPE_JOIN_MISMATCH 805
#define ACTION_IN_TRAN_BL_ERROR		900
#define TASK_IN_ACTION_BL_ERROR		901
#define GOTO_IN_ACTION_BL_ERROR     902
#define NOT_BOOL_EXPR				903
#define WRONG_STMT_IN_ACTION_BLOCK		2100
#define WRONG_STMT_USED_IN_DECI_BLOCK	2101
#define NON_DETERMINISTIC_LINK_ERROR	2102
#define UNREACHABLE_STMT_ERROR			2103
#define WRONG_SYMBOL_USED_IN_EXPAND_STMT 2104

#define WRONG_OP_ERROR				904





#define OP_PLU		200		//  +
#define OP_MIN		201		//  -
#define OP_MUL		202		//  *
#define OP_DVI		203		//  /

#define OP_GRE		204		//	>
#define OP_LES		205		//  <
#define OP_GRE_EQU	206		//  >=
#define OP_LES_EQU	207		//  <=
#define OP_EQU		208     //  ==
#define OP_NOT_EQU	209     //  !=

#define OP_ASI		216		//  =
#define OP_PLU_PLU	210     //  ++ 아직 지원안함
#define OP_MIN_MIN	211     //  -- 아직 지원안함
#define OP_PLU_ASI	212		//  +=
#define OP_MIN_ASI	213     //  -=
#define OP_MUL_ASI	214		//  *=
#define OP_DVI_ASI	215		//  /=




//ERROR throw
#define NO_TASK_TO_PERFORM		1000
#define NO_BEHAVIOR_TO_PERFORM	1001
#define TASK_NAME_DUPLICATED	1002

static bool isComputable(unsigned int op, unsigned int lt, unsigned int rt)
{
	if(lt==TYP_INT || lt==TYP_SHORT || lt==TYP_LONG || lt==TYP_FLOAT|| lt==TYP_DOUBLE){
		if(rt==TYP_INT || rt==TYP_SHORT || rt==TYP_LONG || rt==TYP_FLOAT|| rt==TYP_DOUBLE ||rt ==TYP_ENUM){
			return true;
		}
	}
	//string에 대해서만
	else if(op==OP_PLU || op==OP_ASI){
		if(lt==TYP_STRING && rt==TYP_STRING)
			return true;
	}

	return false;
}

/**
* 왼쪽으로 오른쪽 값을 대입
*/
static int assignable(unsigned int lt, unsigned int rt)
{
	if(lt == rt) return 0;
	if(isComputable(OP_ASI, lt, rt)){
		if(lt == TYP_INT && rt == TYP_ENUM) // int a; a = color.blue;
			return 0;
		else if(lt>rt)
			return 0;
		else
			return TYP_RVALUE_IS_BIGGER;
	}
	//if(lt != rt)
	return SYM_TYPE_MISS_MATCHED;
}

static unsigned int getStateMod(const std::string& str)
{
	if(str.compare("initial")==0)
		return INITIAL_STATE;
	if(str.compare("goal")==0)
		return GOAL_STATE;

	return UNDEFINED_SYMBOL;
}

static unsigned int getJoinType(const std::string& str)
{
	if(str.compare("andjoin")==0)
		return AND_CONNECTOR;
	if(str.compare("orjoin")==0)
		return OR_CONNECTOR;
	if(str.compare("xorjoin")==0)
		return XOR_CONNECTOR;
	if(str.compare("and")==0)
		return AND_CONNECTOR;
	if(str.compare("or")==0)
		return OR_CONNECTOR;

	return UNDEFINED_SYMBOL;
}

static unsigned int getSynchType(const std::string& str)
{
	if(str.compare("asynch")==0)
		return ASYNCHRO;
	if(str.compare("synch")==0)
		return SYNCHRO;

	return UNDEFINED_SYMBOL;
}

static unsigned int getConnType(const std::string& str)
{
	if(str.compare("conexer")==0)
		return CONEXER;
	if(str.compare("seqexer")==0)
		return SEQEXER;

	return UNDEFINED_SYMBOL;
}

static unsigned int getType(const std::string& str)
{
	if(str.compare("bool")==0)
		return TYP_BOOL;
	if(str.compare("int")==0)
		return TYP_INT;
	if(str.compare("char")==0)
		return TYP_CHAR;
	if(str.compare("string")==0)
		return TYP_STRING;
	if(str.compare("byte")==0)
		return TYP_BYTE;
	if(str.compare("short")==0)
		return TYP_SHORT;
	if(str.compare("long")==0)
		return TYP_LONG;
	if(str.compare("float")==0)
		return TYP_FLOAT;
	if(str.compare("double")==0)
		return TYP_DOUBLE;
	if(str.compare("void")==0)
		return TYP_VOID;

	return TYP_NOT_PRIMITIVE;
}

static std::string getTypeStr(unsigned int type)
{
	if(type==TYP_BOOL)
		return "bool";
	if(type==TYP_INT)
		return "int";
	if(type==TYP_CHAR)
		return "char";
	if(type==TYP_STRING)
		return "std::string";
	if(type==TYP_BYTE)
		return "byte";
	if(type==TYP_SHORT)
		return "short";
	if(type==TYP_LONG)
		return "long";
	if(type==TYP_FLOAT)
		return "float";
	if(type==TYP_DOUBLE)
		return "double";
	if(type==TYP_VOID)
		return "void";

	return "TYPE_NOT_DEFINED";
}

static unsigned int getSysType(const std::string& str)
{
	if(str.compare("STATE_TIME")==0)
		return TYP_SYS_STATETIME;
	if(str.compare("BEHAVIOR_TIME")==0)
		return TYP_SYS_BEHAVIORTIME;
	if(str.compare("TASK_TIME")==0)
		return TYP_SYS_TASKTIME;

	return UNDEFINED_SYMBOL;
}

static unsigned int computationType(unsigned int lt, unsigned int rt)
{
	if(lt==TYP_INT){
		if(rt==TYP_INT)		return TYP_INT;
		if(rt==TYP_SHORT)	return TYP_INT;
		if(rt==TYP_LONG)	return TYP_LONG;
		if(rt==TYP_FLOAT)	return TYP_FLOAT;
		if(rt==TYP_DOUBLE)	return TYP_DOUBLE;
	}
	if(lt==TYP_SHORT){
		if(rt==TYP_INT)		return TYP_INT;
		if(rt==TYP_SHORT)	return TYP_SHORT;
		if(rt==TYP_LONG)	return TYP_LONG;
		if(rt==TYP_FLOAT)	return TYP_FLOAT;
		if(rt==TYP_DOUBLE)	return TYP_DOUBLE;
	}
	if(lt==TYP_LONG){
		if(rt==TYP_INT)		return TYP_LONG;
		if(rt==TYP_SHORT)	return TYP_LONG;
		if(rt==TYP_LONG)	return TYP_LONG;
		if(rt==TYP_FLOAT)	return TYP_FLOAT;
		if(rt==TYP_DOUBLE)	return TYP_DOUBLE;
	}
	if(lt==TYP_FLOAT){
		if(rt==TYP_INT)		return TYP_FLOAT;
		if(rt==TYP_SHORT)	return TYP_FLOAT;
		if(rt==TYP_LONG)	return TYP_FLOAT;
		if(rt==TYP_FLOAT)	return TYP_FLOAT;
		if(rt==TYP_DOUBLE)	return TYP_DOUBLE;
	}
	if(lt==TYP_DOUBLE){
		if(rt==TYP_INT)		return TYP_DOUBLE;
		if(rt==TYP_SHORT)	return TYP_DOUBLE;
		if(rt==TYP_LONG)	return TYP_DOUBLE;
		if(rt==TYP_FLOAT)	return TYP_DOUBLE;
		if(rt==TYP_DOUBLE)	return TYP_DOUBLE;
	}
	if(lt==TYP_STRING){
		if(rt==TYP_STRING)	return TYP_STRING;
	}

	return TYP_UNCOMPATIBLE;
}


static unsigned int getOpType(const std::string& str)
{
	if(str.compare("+")==0)	return OP_PLU;
	if(str.compare("-")==0)	return OP_MIN;
	if(str.compare("*")==0)	return OP_MUL;
	if(str.compare("/")==0)	return OP_DVI;

	if(str.compare(">")==0)	return OP_GRE;
	if(str.compare("<")==0)	return OP_LES;
	if(str.compare(">=")==0)return OP_GRE_EQU;
	if(str.compare("<=")==0)return OP_LES_EQU;
	if(str.compare("==")==0)return OP_EQU;
	if(str.compare("!=")==0)return OP_NOT_EQU;

	if(str.compare("=")==0)	return OP_ASI;
	if(str.compare("++")==0)return OP_PLU_PLU;
	if(str.compare("--")==0)return OP_MIN_MIN;
	if(str.compare("+=")==0)return OP_PLU_ASI;
	if(str.compare("-=")==0)return OP_MIN_ASI;
	if(str.compare("*=")==0)return OP_MUL_ASI;
	if(str.compare("/=")==0)return OP_DVI_ASI;

	return UNDEFINED_SYMBOL;
}

static std::string getOpStr(unsigned int type)
{
	if(type == OP_PLU)		return std::string("+");
	if(type == OP_MIN)		return std::string("-");
	if(type == OP_MUL)		return std::string("*");
	if(type == OP_DVI)		return std::string("/");

	if(type == OP_GRE)		return std::string(">");
	if(type == OP_LES)		return std::string("<");
	if(type == OP_GRE_EQU)	return std::string(">=");
	if(type == OP_LES_EQU)	return std::string("<=");
	if(type == OP_EQU)		return std::string("==");
	if(type == OP_NOT_EQU)	return std::string("!=");

	if(type == OP_ASI)		return std::string("=");
	if(type == OP_PLU_PLU)	return std::string("++");
	if(type == OP_MIN_MIN)	return std::string("--");
	if(type == OP_PLU_ASI)	return std::string("+=");
	if(type == OP_MIN_ASI)	return std::string("-=");
	if(type == OP_MUL_ASI)	return std::string("*=");
	if(type == OP_DVI_ASI)	return std::string("/=");

	return std::string("TYPE_UNKNOWN");
}