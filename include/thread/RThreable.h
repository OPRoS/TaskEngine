/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

class RThreable
{
public:
	RThreable(void);
	virtual ~RThreable(void);

	virtual int run();
	virtual int suspended();
	virtual int resumed();
	virtual int stopped();	
};