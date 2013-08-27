/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "ds/EList.h"
#include <string>
#include "Gstmt.h"
#include "util/Utility.h"

/*
statement인데 블럭구조를 가지는 것들, with, seq, par
*/
class GstmtBlock : public Gstmt
{
private:	
	int				m_mod;
	int				m_period; //run, with에만 있는 속성
	std::string		m_blockID;
	EList<Gstmt*>*	m_stmtlist;

public:
	GstmtBlock(std::string file, int eol, int blockid, std::string blockname, std::string blockpath);
	GstmtBlock(std::string file, int blockid);
	virtual ~GstmtBlock(void);

	void setStmts(EList<Gstmt*>*);
	void addStmt(Gstmt*);
	void setModifier(int);
	void setPeriod(int);


	int				getModifier(); //STMTBLOCK, WITH, PAR, SEQ
	int				getPeriod();
	std::string		getBlockID();
	EList<Gstmt*>*	getStmts();

	char*	toString(){return NULL;};
	void	print(unsigned int);

	static int s_PAR;
	static int s_SEQ;
	static int s_BLO;
	static int s_WITH;
};
