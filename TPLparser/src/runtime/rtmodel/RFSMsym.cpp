/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMsym.h"

RFSMsym::RFSMsym(std::string blockPath, std::string blockname, std::string name, std::string file, int line)
	:RFSMvalue(blockPath, blockname, file, line), 
	 RFSMnamedItem(name)
{
}

RFSMsym:: ~RFSMsym(void)
{
}
