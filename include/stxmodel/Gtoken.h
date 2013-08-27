/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "stxmodel/Ggram.h"
#include "ds/Estring.h"
#include "ds/EList.h"

class Gtoken : public Ggram
{
private:	
	std::string		m_data;  //token
	unsigned int	m_type;	
	//unsigned int	m_line;	

public:
	Gtoken(std::string, std::string, std::string, std::string, unsigned int, unsigned int);
	Gtoken(void);
	virtual ~Gtoken(void);

	void	print(unsigned int);
	char*	toString(){return NULL;};
	const char*	toStr();

	void setData(std::string);
	std::string		getData(void);
	unsigned int	getType(void);
	//unsigned int	getLine(void);
	void print();
};
