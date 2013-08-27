/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "thread/RThreable.h"

RThreable::RThreable(void)
{	
}

RThreable:: ~RThreable(void)
{
}

int RThreable::run()
{
	return 0;
}

int RThreable::suspended()
{
	return 0;
}

int RThreable::resumed()
{
	return 0;
}

int RThreable::stopped()
{
	return 0;
}