/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "GstmtBlock.h"
#include "Gtoken.h"

class Gworker : public GstmtBlock
{
private:
	Gtoken* m_name;
	Gtoken* m_desc;
	Gtoken* m_startBhv;

public:
	Gworker(std::string file, int eol, int blockid, std::string blockname, std::string blockpath);
	virtual ~Gworker(void);

	void setName(Gtoken*);
	void setDesc(Gtoken*);
	void setStartBhv(Gtoken*);

	Gtoken* getName();
	Gtoken* getDesc();
	Gtoken* getStartBhv();

	void	print(unsigned int);
	char*	toString(){return NULL;};
};
