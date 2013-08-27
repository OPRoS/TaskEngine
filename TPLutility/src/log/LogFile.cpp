/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "log/LogFile.h"

LogFile::LogFile(int maxsize)
{
	//m_max = 10000;
	m_max = maxsize;
	m_count = 0;
}

LogFile::LogFile(char* name, int maxsize)
{
	//m_max = 10000;
	m_max = maxsize;
	m_count = 0;

	m_title = std::string(name);
	std::string fname(m_title);
	fname.append(".log");
	m_logfile.open(fname.c_str());
}

LogFile:: ~LogFile(void)
{
	 m_logfile.close();
}

void LogFile::printHead(const char* prefix, const char* func, const char* file, long line)
{
	//printf("[%s: %s] %s(%d) ", prefix, m_name, file, line, func);

	if(m_count >= m_max){
		m_logfile.close();
		std::string fname(m_title);
		fname.append(".log");
		m_logfile.open(fname.c_str());
		m_count = 0;
	}

	time_t timer= time(NULL);
	struct tm *t = localtime(&timer);

	int year	= t->tm_year+1900;
	int mon		= t->tm_mon+1;
	int hour	= t->tm_hour;
	int min		= t->tm_min;
	int sec		= t->tm_sec;

	m_logfile <<"["<<prefix<<": " << m_title.c_str()<<"] "<<year<<"."<<mon<<"."<<t->tm_mday<<" "<<hour<<":"<<min<<":"<<sec<<" ";
	m_logfile.flush();

	m_count++;
}


void LogFile::writeMsg(const char* input, ... )
{
    va_list args;
    va_start(args, input );
    char buffer[1024];
    //vsprintf_s( buffer, 1024, input, args ); //vsnprintf, vaprintf
    vsnprintf( buffer, 1024, input, args ); //vsnprintf, vaprintf

	//printf("%s\n", buffer);
	m_logfile <<buffer<<std::endl;
	m_logfile.flush();
}
