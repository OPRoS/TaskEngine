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
	std::string		m_blockname; //bhv1.state1.entry(�ڽ��� ���� ��ġ)

	int				m_line;			//������ ���ǵ� ������ ���ι�ȣ
	std::string		m_file;			//������ ���ǵ� ���ϸ�

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
