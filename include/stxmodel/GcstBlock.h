/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include <string>
#include "stxmodel/Gtoken.h"
#include "stxmodel/Ginclude.h"
#include "stxmodel/GstmtBlock.h"
#include "stxmodel/GlocalVar.h"

class GcstBlock : public GstmtBlock
{
private:	
	Gtoken*			m_name;
	GstmtBlock*		m_const;
	GstmtBlock*		m_dest;
	bool			m_isMission;

public:
	GcstBlock(std::string file, int eol, int blockid, std::string blockname, std::string blockpath);
	virtual ~GcstBlock(void);

	/*
	Realization of Member functions in Ggram */
	void	print(unsigned int);
	char*	toString(){return NULL;};

	void	setName			(Gtoken*);	
	void	setConstruct	(GstmtBlock*);
	void	setDestruct		(GstmtBlock*);
	void	setCstVars		(EList<Gstmt*>* vars);  //GstmtBlock의 stmt를 저장하는 곳에 로컬변수(statement)를 저장
	void	addCstVar		(GlocalVar* lvar);
	void	setAsMission	(void);
	
	bool			isMission	(void);
	Gtoken*			getName		(void);
	GstmtBlock*		getConstruct(void);
	GstmtBlock*		getDestruct	(void);
	EList<Gstmt*>*	getCstVars	(void);
};
