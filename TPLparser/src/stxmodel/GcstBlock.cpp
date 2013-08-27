/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/GcstBlock.h"

GcstBlock::GcstBlock(std::string file, int eol, int blockid, std::string blockname, std::string blockpath): GstmtBlock(file, eol, blockid, blockname, blockpath)
{
	m_name		= NULL;
	m_dest		= NULL;
	m_const		= NULL;
	m_isMission = false;
}

GcstBlock:: ~GcstBlock(void)
{
	if(m_const!= NULL)	
		delete m_const;
	if(m_dest != NULL)	
		delete m_dest;
	if(m_name != NULL)	
		delete m_name;	
		
	m_name = NULL;
	m_dest = NULL;
	m_const = NULL;
}

void GcstBlock::setName(Gtoken* name)
{
	m_name = name;
}

void GcstBlock::setCstVars(EList<Gstmt*>* vars)
{
	setStmts(vars);
}

void GcstBlock::addCstVar(GlocalVar* lvar)
{
	addStmt(lvar);
}

void GcstBlock::setConstruct(GstmtBlock* cons)
{
	m_const = cons;
}

void GcstBlock::setDestruct(GstmtBlock* dest)
{
	m_dest = dest;
}

void GcstBlock::setAsMission()
{
	m_isMission = true;
}
	
bool GcstBlock::isMission()
{
	return m_isMission;
}

EList<Gstmt*>* GcstBlock::getCstVars()
{	
	return getStmts();
}

GstmtBlock* GcstBlock::getConstruct()
{
	return m_const;
}

GstmtBlock* GcstBlock::getDestruct()
{
	return m_dest;
}

Gtoken* GcstBlock::getName()
{
	return m_name;
}

void GcstBlock::print(unsigned int indent)
{

}
