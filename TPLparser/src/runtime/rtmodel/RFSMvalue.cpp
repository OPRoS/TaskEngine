/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMvalue.h"
#include "runtime/rtmodel/RFSMmulti.h"
#include "runtime/rtmodel/RFSMadd.h"
#include "runtime/rtmodel/RFSMasmt.h"
#include "runtime/rtmodel/RFSMcallFunc.h"
#include "runtime/rtmodel/RFSMcondValue.h"
#include "runtime/rtmodel/RFSMliteral.h"
#include "runtime/rtmodel/RFSMsign.h"
#include "runtime/rtmodel/RFSMvariable.h"
#include "runtime/rtmodel/RFSMenum.h"
#include "runtime/rtmodel/RFSMsystem.h"

class RFSMmulti;
class RFSMadd;
class RFSMasmt;
class RFSMcallFunc;
class RFSMcondValue;
class RFSMliteral;
class RFSMsign;
class RFSMvariable;
class RFSMenum;
class RFSMsystem;

RFSMvalue::RFSMvalue(std::string blockpath, std::string blockname, std::string file, int line)
	:RFSMBlockInfo(blockpath, blockname, file, line)
{	
}

RFSMvalue:: ~RFSMvalue(void)
{
}

void RFSMvalue::setType(unsigned int type)
{
	m_type = type;
}

std::string RFSMvalue::toString()
{
	return std::string("RFSMvalue");
}

std::string RFSMvalue::toString(RFSMvalue* value)
{

	if(typeid(*value)==typeid(RFSMmulti) ){
		RFSMmulti* co = (RFSMmulti*)value;
		return co->toString();
	}
	if(typeid(*value)==typeid(RFSMadd) ){
		RFSMadd* co = (RFSMadd*)value;
		return co->toString();
	}
	if(typeid(*value)==typeid(RFSMasmt) ){
		RFSMasmt* co = (RFSMasmt*)value;
		return co->toString();
	}
	if(typeid(*value)==typeid(RFSMcallFunc) ){
		RFSMcallFunc* co = (RFSMcallFunc*)value;
		return co->toString();
	}
	if(typeid(*value)==typeid(RFSMcondValue) ){
		RFSMcondValue* co = (RFSMcondValue*)value;
		return co->toString();
	}
	if(typeid(*value)==typeid(RFSMliteral) ){
		RFSMliteral* co = (RFSMliteral*)value;
		return co->toString();
	}
	if(typeid(*value)==typeid(RFSMsign)){
		RFSMsign* co = (RFSMsign*)value;
		return co->toString();
	}
	if(typeid(*value)==typeid(RFSMvariable) ){
		RFSMvariable* co = (RFSMvariable*)value;
		return co->toString();
	}
	if(typeid(*value)==typeid(RFSMenum)){
		RFSMenum* co = (RFSMenum*)value;
		return co->toString();
	}
	if(typeid(*value)==typeid(RFSMsystem)){
		RFSMsystem* co = (RFSMsystem*)value;
		return co->toString();
	}

	return std::string("");
}
