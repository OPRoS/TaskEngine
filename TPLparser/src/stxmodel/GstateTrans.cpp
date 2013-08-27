/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/GstateTrans.h"

GstateTrans::GstateTrans(std::string file, int eol, int blockid, std::string blockname, std::string blockpath) : GstmtBlock(file, eol, blockid, blockname, blockpath)
{
	//m_ifstmt = NULL;	
	//m_iflist = NULL;	
}

GstateTrans:: ~GstateTrans(void)
{
	/*
	if(m_iflist != NULL){
		while(m_iflist->getHead() !=NULL){
			m_iflist->delHead();
		}
		delete m_iflist;
	}
	*/
	//delete m_ifstmt;
}

/*
void GstateTrans::setIfstmt(GstmtIf *ifstmt)
{
	if(m_ifstmt != NULL)
		delete m_ifstmt;

	m_ifstmt = ifstmt;
}
*/

//void GstateTrans::addIfstmt(GstmtIf *ifstmt)
//{
//	if(m_iflist == NULL)
//		m_iflist = new EList<GstmtIf*>();
//	m_iflist->addTail(ifstmt);
//}


//EList<GstmtIf*>* GstateTrans::getIfstmts()
//{
//	return m_iflist;
//}

void GstateTrans::print(unsigned int indent)
{
	titlePrint(indent, std::string("TRANS"));

	//if(m_ifstmt != NULL)
	//	m_ifstmt->print(indent +1);

	//if(m_iflist->moveHead()){
	//	do m_iflist->getCurObject()->print(indent +1);
	//	while(m_iflist->moveNext());
	//}
	print(indent +1);

}
