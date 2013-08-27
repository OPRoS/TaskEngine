/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/symtab/TABaction.h"

TABaction::TABaction(std::string name, EList<TABparameter*>* params):TABcall(name, params)
{
}

TABaction:: ~TABaction(void)
{
}
