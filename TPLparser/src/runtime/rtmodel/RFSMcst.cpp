/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMcst.h"

RFSMcst::RFSMcst(std::string name, int nameLine, std::string blockID): RFSMnamedItem(name)
{
	m_nameLine	= nameLine;
//	m_blockID	= blockID;
	m_const		= NULL;
	m_dest		= NULL;
	m_varAsmts	= NULL;
}

RFSMcst:: ~RFSMcst(void)
{
	if(m_const != NULL){
		/*while(m_const->moveHead()){
			RFSMstmt* obj = m_const->getCurObject();
			delete obj;
			m_const->delHead();
		}*/
		for(unsigned int i=0 ; i<m_const->size(); i++){
			delete (*m_const)[i];
		}
		delete m_const;
	}
	
	if(m_dest != NULL){
		/*while(m_dest->moveHead()){
			RFSMstmt* obj = m_dest->getCurObject();
			delete obj;
			m_dest->delHead();
		}*/
		for(unsigned int i=0 ; i<m_dest->size(); i++){
			delete (*m_dest)[i];
		}
		delete m_dest;
	}

	if(m_varAsmts != NULL){
		/*while(m_varAsmts->moveHead()){
			RFSMstmt* obj = m_varAsmts->getCurObject();
			delete obj;
			m_varAsmts->delHead();
		}*/
		for(unsigned int i=0 ; i<m_varAsmts->size(); i++){
			delete (*m_varAsmts)[i];
		}
		delete m_varAsmts;
	}
}


void RFSMcst::setConstruct(std::vector<RFSMstmt*>* cons)
{
	m_const = cons;
}

void RFSMcst::setDestruct(std::vector<RFSMstmt*>* dest)
{
	m_dest = dest;
}

void RFSMcst::addVarAsmt(RFSMstmt* asmt)
{
	if(m_varAsmts == NULL)
		m_varAsmts = new std::vector<RFSMstmt*> ();
	m_varAsmts->push_back(asmt);
}

std::vector<RFSMstmt*>* RFSMcst::getVarAsmts()
{
	return m_varAsmts;
}

std::vector<RFSMstmt*>* RFSMcst::getConstruct()
{
	return m_const;
}

std::vector<RFSMstmt*>* RFSMcst::getDestruct()
{
	return m_dest;
}

/*
std::string RFSMcst::getBlockID()
{
	return m_blockID;
}
*/
void RFSMcst::setEOLconst(int eol)
{
	m_EOLconst = eol;
}

void RFSMcst::setEOLdest(int eol)
{
	m_EOLdest = eol;
}

int  RFSMcst::getNameLine()
{
	return m_nameLine;
}

int  RFSMcst::getEOLconst()
{
	return m_EOLconst;
}

int  RFSMcst::getEOLdest()
{
	return m_EOLdest;
}