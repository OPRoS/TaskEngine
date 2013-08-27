/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "Gtoken.h"
#include "Gstmt.h"
#include "GstmtExpr.h"

class GlocalVar : public Gstmt
{
protected:
	Gtoken*		m_type;
	Gtoken*		m_name;
	bool		m_isPrim;
	GstmtExpr*	m_rval;

public:
	GlocalVar(Gtoken*, Gtoken*, std::string, int, std::string, std::string);
	GlocalVar(std::string, int, std::string, std::string);
	virtual ~GlocalVar(void);

	void setType(Gtoken*);
	void setName(Gtoken*);
	void setAsPrim();
	void setValueExpr(GstmtExpr*);

	Gtoken*		getType();
	Gtoken*		getName();
	bool		isPrim();
	GstmtExpr*	getValueExpr();

	char*	toString(){return "";};
	void	print(unsigned int){};

};
