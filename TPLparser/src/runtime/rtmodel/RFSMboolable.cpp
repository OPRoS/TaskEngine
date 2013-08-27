/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMboolable.h"
#include "runtime/rtmodel/RFSMcallFunc.h"
#include "runtime/rtmodel/RFSMliteral.h"
#include "runtime/rtmodel/RFSMand.h"
#include "runtime/rtmodel/RFSMor.h"
#include "runtime/rtmodel/RFSMRelational.h"
#include "runtime/rtmodel/RFSMvariable.h"

class RFSMcallFunc;
class RFSMliteral;
class RFSMand;
class RFSMor;
class RFSMRelational;
class RFSMvariable;

RFSMboolable::RFSMboolable(std::string className): RFSMObject(className)
{
	m_hasNot = false;
}

RFSMboolable:: ~RFSMboolable(void)
{
}


void RFSMboolable::setHasNot()
{
	m_hasNot = true;
}

bool RFSMboolable::hasNot()
{
	return m_hasNot;
}

std::string RFSMboolable::toString()
{
	return std::string("RFSMboolable");
}

std::string RFSMboolable::toString(RFSMboolable* value)
{
	if(value->getClassName().compare(CLS_RFSMcallFunc)==0){
		RFSMcallFunc* co = (RFSMcallFunc*)value;
		return co->toString();
	}
	if(value->getClassName().compare(CLS_RFSMliteral)==0){
		RFSMliteral* co = (RFSMliteral*)value;
		return co->toString();
	}
	if(value->getClassName().compare(CLS_RFSMand)==0){
		RFSMand* co = (RFSMand*)value;
		return co->toString();
	}
	if(value->getClassName().compare(CLS_RFSMor)==0){
		RFSMor* co = (RFSMor*)value;
		return co->toString();
	}
	if(value->getClassName().compare(CLS_RFSMRelational)==0){
		RFSMRelational* co = (RFSMRelational*)value;
		return co->toString();
	}
	if(value->getClassName().compare(CLS_RFSMvariable)==0){
		RFSMvariable* co = (RFSMvariable*)value;
		return co->toString();
	}
	return std::string("");
}
