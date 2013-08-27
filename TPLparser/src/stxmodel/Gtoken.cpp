/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/Gtoken.h"

Gtoken::Gtoken(std::string fname, std::string blockname, std::string blockpath, std::string data, unsigned int line, unsigned int type)
	:Ggram(fname, line, blockname, blockpath)
{
	setData(data);	
	m_type = type;	
	//m_line = line;
}

Gtoken::Gtoken(void):Ggram("", -1, "", "")
{
}

Gtoken:: ~Gtoken(void)
{
}

std::string Gtoken::getData()
{
	return m_data;
}
/*
std::string Gtoken::getBlockPath()

{
	return m_blockPath;
}

std::string Gtoken::getBlockName()
{
	return m_blockname;
}
*/
/*
unsigned int Gtoken::getLine()
{
	return m_line;
}
*/

unsigned int Gtoken::getType()
{
	return m_type;
}

void Gtoken::print(unsigned int indent)
{
	for (unsigned int i=0; i<indent; i++) 
		printf("|  ");
	printf("|- {%s, %s, %d, %d}\n", m_data.c_str(), getBlockPath().c_str(), getEndLine(), m_type);
}

void Gtoken::print()
{
	printf("{%s, %s, %d, %d}\n", m_data.c_str(), getBlockPath().c_str(), getEndLine(), m_type);
}
/*
const char* Gtoken::toStr()
{
	
	std::string *str = new std::string(m_data.c_str());
	str->append(":");
	std::string linestr = Estring::toString(m_line);
	str->append(linestr);
	return str->c_str();
}
*/

void Gtoken::setData(std::string data)
{
	Estring::replaceAll(data,"\"","");
	data = Estring::trim(data);
	m_data = data;
}