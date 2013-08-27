/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
************************************************************************************************/

#pragma once

#include <string>
#include "ds/Estring.h"

class RFSMBlockInfo
{
private:		
	std::string		m_blockid;   //0.1.2
	std::string		m_blockname; //bhv1.state1.entry(자신이 속한 위치)

	int				m_line;			//변수가 정의된 파일의 라인번호
	std::string		m_file;			//변수가 정의된 파일명

public:
	RFSMBlockInfo(std::string, std::string, std::string file, int line);
	virtual ~RFSMBlockInfo(void);

	void			setBlockName	(std::string);
	void			setBlockID		(std::string);
	void			setEndLine		(int);
	std::string		getBlockName	(void);
	std::string     getBlockID		(void);
	int				getEndLine		(void);
	std::string		getFileName		(void);
};
