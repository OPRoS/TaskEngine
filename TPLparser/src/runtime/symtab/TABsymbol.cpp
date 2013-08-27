/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/symtab/TABsymbol.h"

//TABsymbol::TABsymbol(void)
//{
//}

TABsymbol::TABsymbol(std::string bPath, std::string blockname, std::string name, unsigned int type, unsigned int mod, std::string value)
: RFSMnamedItem(name)
{
	m_blockPath = bPath;
	//std::vector<std::string> temp = Estring::split(bPath, ".");
	//if(temp.size()>0){
	//	m_blockID = *(temp.end());
	//}
	m_blockName = blockname;
	m_type = type;
	m_value = value;
	m_mod = mod;
}

TABsymbol:: ~TABsymbol(void)
{
	//GmodelVar* 는 나중에 Eparser에서 다 지워진다. 그냥 내버려두자..
}

std::string TABsymbol::getBlockPath()
{
	return m_blockPath;
}

std::string TABsymbol::getBlockName()
{
	return m_blockName;
}

unsigned int TABsymbol::getSymbolType()
{
	return m_type;
}

std::string TABsymbol::getValueStr()
{
	return m_value;
}

unsigned int TABsymbol::getSymbolMod()
{
	return m_mod;
}
