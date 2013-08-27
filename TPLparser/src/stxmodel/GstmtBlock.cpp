/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/GstmtBlock.h"

int GstmtBlock::s_PAR 	= 1;
int GstmtBlock::s_SEQ	= 2;
int GstmtBlock::s_BLO	= 3;
int GstmtBlock::s_WITH	= 4;

GstmtBlock::GstmtBlock(std::string file, int eol, int blockid, std::string blockname, std::string blockpath):Gstmt(STMT_Block, file, eol, blockname, blockpath)
{
	m_stmtlist = NULL;
	m_blockID = int2str(blockid);
	
}

GstmtBlock::GstmtBlock(std::string file, int blockid):Gstmt(STMT_Block, file)
{
	m_stmtlist = NULL;
	m_blockID = int2str(blockid);
}

GstmtBlock:: ~GstmtBlock(void)
{
	if(m_stmtlist != NULL){
		while(m_stmtlist->getHead() !=NULL && m_stmtlist->moveHead()){
			Gstmt* obj = m_stmtlist->getCurObject();
			delete obj;
			m_stmtlist->delHead();
		}
		delete m_stmtlist;
	}

	m_stmtlist = NULL;
}

void GstmtBlock::setStmts(EList<Gstmt*> *stmts)
{
	if(m_stmtlist != NULL){
		delete m_stmtlist;
	}
	m_stmtlist = stmts;
}

void GstmtBlock::addStmt(Gstmt* stmt)
{
	if(m_stmtlist == NULL){
		m_stmtlist = new EList<Gstmt*>();
	}
	m_stmtlist->addTail(stmt);
}

void GstmtBlock::print(unsigned int indent)
{
	titlePrint(indent, std::string("STMTS"));

	if(m_stmtlist !=NULL && m_stmtlist->moveHead()){
		do{
			Gstmt *stmt = m_stmtlist->getCurObject();
			stmt->print(indent+1);
		}
		while(m_stmtlist->moveNext());
	}
}

std::string GstmtBlock::getBlockID()
{
	return m_blockID;
}

void GstmtBlock::setModifier(int mod)
{
	m_mod = mod;
}

int GstmtBlock::getModifier()
{
	return m_mod;
}

EList<Gstmt*>* GstmtBlock::getStmts()
{
	if(m_stmtlist == NULL)
		return NULL;
	return m_stmtlist;
}