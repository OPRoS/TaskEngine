/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/GstmtCall.h"

GstmtCall::GstmtCall(std::string file, int eol, std::string blockname, std::string blockpath):Gstmt(STMT_CallFunc, file, eol, blockname, blockpath)
{
	m_call = NULL;
}

GstmtCall:: ~GstmtCall()
{
	if(m_call != NULL){
		delete m_call;
		m_call = NULL;
	}
}

void GstmtCall::setCall(Gcall* call)
{
	m_call = call;
}

Gcall* GstmtCall::getCall()
{
	return m_call;
}

void GstmtCall::print(unsigned int indent)
{
	if(m_call != NULL)
		m_call->print(indent);
}