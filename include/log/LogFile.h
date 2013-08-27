/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "log/Log.h"
#include <fstream>
#include <time.h>

class LogFile : public Log
{
private:
	std::string		m_title;
	std::ofstream	m_logfile;
	
	int	m_count;
	int	m_max;

public:

	LogFile(int);
	LogFile(char*, int);

	virtual ~LogFile(void);

	void printHead(const char* prefix, const char* func, const char* file, long line); //overload
	void writeMsg(const char* input, ...); //LogFile�� �ַ� �����ͷ� �������� ()�� overload�ϸ� ��ũ�μ� ����ϱ� ��ٷο� ���� ����
};
