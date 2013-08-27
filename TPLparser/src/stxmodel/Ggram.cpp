/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/Ggram.h"

Ggram::Ggram(std::string file, int eol, std::string blockname, std::string blockpath)
{
	m_file		= file;
	m_eol		= eol;	
	m_blockname	= blockname;
	m_blockpath	= blockpath;	
}

Ggram::Ggram(std::string file)
{
	m_file = file;
}

Ggram:: ~Ggram(void)
{
}

void Ggram::setEndLine(unsigned int line)
{
	m_eol = line;
}

void Ggram::setBlockName(std::string blockname)
{
	m_blockname = blockname;
}

void Ggram::setBlockPath(std::string blockpath)
{
	m_blockpath = blockpath;
}

int Ggram::getEndLine()
{
	return m_eol;
}

std::string Ggram::getBlockName()
{
	return m_blockname;
}

std::string Ggram::getBlockPath()
{
	return m_blockpath;
}

std::string Ggram::getBID()
{
	std::vector<std::string> pa = Estring::split(m_blockpath, ".");
	if(pa.size()>0)
		return pa[pa.size()-1];
	return "";
}

std::string Ggram::getFileName()
{
	return m_file;
}

void Ggram::titlePrint(unsigned int indent, std::string title)
{
	for (unsigned int i=0; i<indent; i++) 
		printf("|  ");
	printf("|- [%s]\n", title.c_str());
}