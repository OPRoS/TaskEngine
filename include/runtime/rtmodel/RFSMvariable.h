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
#include "runtime/rtmodel/RFSMboolable.h"
#include "runtime/rtmodel/RFSMsym.h"

class RFSMvariable : public RFSMsym, public RFSMboolable
{
private:	
	//int					m_line;			//변수가 정의된 파일의 라인번호
	//std::string			m_file;			//변수가 정의된 파일명
	unsigned int		m_varMode;

public:
	RFSMvariable(std::string, std::string, std::string, unsigned int type, unsigned int varmode, std::string file, int line);
	virtual ~RFSMvariable(void);

	//std::string		getBlockPath();
	//std::string		getBlockID();
	//std::string		getBlockName();
	unsigned int		getType();
	unsigned int		getMode();
//	int					getLine();
//	std::string			getFileName();
	std::string			toString();
};
