/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/Gparameter.h"

Gparameter::Gparameter(void)
{
	m_name = NULL;
	m_type = NULL;
	m_value = NULL;
	m_isPrim = false;
}

Gparameter:: ~Gparameter(void)
{
	if(m_name != NULL)
		delete m_name;
	if(m_type != NULL) 
		delete m_type;
	if(m_value != NULL) 
		delete m_value;

	m_name = NULL;
	m_type = NULL;
	m_value = NULL;
}

void Gparameter::setName(Gtoken *name)
{
	m_name = name;
}

void Gparameter::setType(Gtoken *type)
{
	m_type = type;
}

void Gparameter::setAsPrim()
{
	m_isPrim =true;
}

Gtoken* Gparameter::getType()
{
	return m_type;
}

Gtoken* Gparameter::getName()
{
	return m_name;
}

void Gparameter::print(unsigned int indent)
{
	//m_name에 있는 titlePrint함수 이용
	m_name->titlePrint(indent, std::string("PARAM"));
	
	if(m_type != NULL)
		m_type->print(indent+1);
	if(m_name != NULL)
		m_name->print(indent+1);
}

