/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/GlocalVar.h"

GlocalVar::GlocalVar(Gtoken* type, Gtoken* name, std::string file, int eol, std::string blockname, std::string blockpath):Gstmt(STMT_VarDecl, file, eol, blockname, blockpath)
{
	m_name = name;
	m_type = type;
	m_isPrim = false;
	m_rval = NULL;
}


GlocalVar::GlocalVar(std::string file, int eol, std::string blockname, std::string blockpath):Gstmt(STMT_VarDecl, file, eol, blockname, blockpath)
{
	m_isPrim = false;
	m_rval = NULL;
}

GlocalVar:: ~GlocalVar(void)
{
	if(m_name != NULL)
		delete m_name;
	if(m_type != NULL)
		delete m_type;
	if(m_rval != NULL)
		delete m_rval;
}
	
void GlocalVar::setType(Gtoken* type)
{
	m_type = type;
}

void GlocalVar::setName(Gtoken* name)
{
	m_name = name;
}

Gtoken* GlocalVar::getName()
{
	return m_name;
}

Gtoken* GlocalVar::getType()
{
	return m_type;
}

void GlocalVar::setAsPrim()
{
	m_isPrim =true;
}


bool GlocalVar::isPrim()
{
	return m_isPrim;
}

void GlocalVar::setValueExpr(GstmtExpr* val)
{
	m_rval = val;
}

GstmtExpr* GlocalVar::getValueExpr()
{
	return m_rval;
}
