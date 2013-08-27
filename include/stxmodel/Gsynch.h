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

class Gsynch : public Gstmt
{
private:	
	Gtoken*		m_name;
	GstmtExpr*	m_time;

public:	
	Gsynch(std::string file, int eol, std::string blockname, std::string blockpath);
	virtual ~Gsynch(void);

	void		setSynchID(GstmtExpr*);
	Gtoken*		getName();
	GstmtExpr*	getSynchID();

	char*	toString(){return "";};
	void	print(unsigned int){};

};
