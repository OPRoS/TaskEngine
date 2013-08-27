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

class Gwaitfor : public Gstmt
{
private:	
	Gtoken*		m_name;
	GstmtExpr*	m_time;

public:	
	Gwaitfor(std::string file, int eol, std::string blockname, std::string blockpath);
	virtual ~Gwaitfor(void);

	void		setTime(GstmtExpr*);
	Gtoken*		getName();
	GstmtExpr*	getTime();

	char*	toString(){return "";};
	void	print(unsigned int){};

};
