/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/GmodelVar.h"

GmodelVar::GmodelVar(unsigned int mode, Gtoken* type, Gtoken* name, std::string file, int eol, std::string blockname, std::string blockpath) : GlocalVar(type, name, file, eol, blockname, blockpath)
{
	m_mode = mode;
}

GmodelVar::GmodelVar(std::string file, int eol, std::string blockname, std::string blockpath): GlocalVar(file, eol, blockname, blockpath)
{
}

GmodelVar:: ~GmodelVar(void)
{
}


void GmodelVar::setMode(unsigned int mode)
{
	m_mode = mode;
}
	
unsigned int GmodelVar::getMode()
{
	return m_mode;
}


GmodelVar GmodelVar::deepCopy()
{
	GmodelVar gVar = GmodelVar(m_mode, m_type, m_name, m_name->getFileName(), getEndLine(), getBlockName(), getBlockPath());
	if(m_isPrim) gVar.setAsPrim();

	return gVar;
}