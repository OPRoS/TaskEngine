/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "GstmtIf.h"
#include "ds/EList.h"
#include "GstmtBlock.h"

class GstateTrans : public GstmtBlock
{
private:	
	//GstmtIf* m_ifstmt;
	//EList<GstmtIf*>*	m_iflist;
	
public:
	GstateTrans(std::string file, int eol, int blockid, std::string blockname, std::string blockpath);
	virtual ~GstateTrans(void);

	void	print(unsigned int);
	char*	toString(){return NULL;};

	//void setIfstmt(GstmtIf*);
	//void addIfstmt(GstmtIf*);
	//EList<GstmtIf*>* getIfstmts();
	
};
