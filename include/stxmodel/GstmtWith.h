/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "util/Utility.h"
#include "GstmtBlock.h"

class GstmtWith: public GstmtBlock
{

public:
	GstmtWith(std::string file, int eol, int blockid, std::string blockname, std::string blockpath);	
	virtual ~GstmtWith(void);	
};