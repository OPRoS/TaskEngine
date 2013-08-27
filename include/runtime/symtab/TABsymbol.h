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
#include "runtime/rtmodel/RFSMnamedItem.h"


class TABsymbol : public RFSMnamedItem
{
private:
	std::string		m_blockPath;	//0.1.2
	std::string     m_blockName;
	unsigned int	m_type;
	std::string		m_value;
	unsigned int	m_mod;			//in, out, local
									//nmame은 부모클래스에 0.1.2.x

public:
	//TABsymbol(void);
	TABsymbol(std::string bPath, std::string blockname, std::string name, unsigned int type, unsigned int mod, std::string value);
	virtual ~TABsymbol(void);
	
	std::string		getBlockPath();
	std::string		getBlockName();
	unsigned int	getSymbolType();
	std::string		getValueStr();
	unsigned int	getSymbolMod();
};
