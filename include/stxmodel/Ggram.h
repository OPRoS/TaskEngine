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
#include <stdio.h>
#include "ds/Estring.h"

class Ggram
{
private:	
	std::string		m_file;
	unsigned int	m_eol;	
	std::string		m_blockpath;
	std::string		m_blockname; //bhv1.state1.entry(자신이 속한 위치)

public:
	Ggram(std::string, int, std::string, std::string);
	Ggram(std::string);
	virtual ~Ggram(void);

	virtual char*	toString()=0;
	virtual void	print(unsigned int)=0;

	void			setBlockName(std::string);
	void			setBlockPath(std::string);
	void			setEndLine(unsigned int);
	std::string		getFileName();
	int				getEndLine();
	std::string		getBlockName();
	std::string     getBlockPath();
	std::string     getBID();
	void			titlePrint(unsigned int, std::string);

};
