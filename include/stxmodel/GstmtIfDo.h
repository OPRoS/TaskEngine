/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "stxmodel/GstmtBlock.h"

class GstmtIfDo : public GstmtBlock
{
public:
	GstmtIfDo(std::string file, int eol, int blockid, std::string blockname, std::string blockpath);
	virtual ~GstmtIfDo(void);
};
