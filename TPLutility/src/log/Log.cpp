/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "log/Log.h"

Log::Log(void)
{
	m_ostream = NULL;
}

Log::Log(char* name, std::ostream* stream)
{
	m_ostream = stream;

	m_name = std::string(name);
	for(unsigned int i=0; i<6;i++){
		m_levelStatus[i] = false;
	}
}

Log:: ~Log(void)
{
}

void Log::setName(std::string name)
{
	m_name = name;
}

void Log::setLevel(unsigned int level, bool activated)
{
	m_levelStatus[level] = activated;
}

void Log::printHead(const char* prefix)
{	
	//Remote일때만 m_ostream이 null이 아니다
	if(m_ostream != NULL)
		(*m_ostream)<<"["<<prefix<<": "<<m_name.c_str()<<"] ";
	else
		printf("[%s: %s] ", prefix, m_name.c_str());
}

void Log::printHead(const char* prefix, const char* func, const char* file, long line)
{
	if(m_ostream != NULL)
		(*m_ostream)<<"["<<prefix<<": "<<m_name.c_str()<<"] "<<file<<"("<<line<<")" ;
	else
		printf("[%s: %s] %s(%d) ", prefix, m_name.c_str(), file, line, func);
}

bool Log::assertR(bool exp, char* file, long line, char* msg)
{
	if(exp){
		if(m_ostream != NULL)
			(*m_ostream)<<file<<"("<<line<<"), "<<msg<<"\r\n" ;
		else
			printf("%s(%d), %s\r\n", file, line, msg);
		return true;
	}
	return false;
}

bool Log::isEnabled(unsigned int level)
{
	return m_levelStatus[level];
}

void Log::operator()(const char* input, ... )
{
    va_list args;
    va_start(args, input );
    char buffer[1024];
	memset(buffer, 0, 1024);
    //vsprintf_s( buffer, 1024, input, args ); //vsnprintf, vaprintf
    vsnprintf( buffer, 1024, input, args ); //vsnprintf, vaprintf

    //std::string to_log( buffer );
    //to_log += "\n";
	
	if(m_ostream != NULL)
		(*m_ostream)<<buffer<<"\r\n";
	else
		printf("%s\r\n", buffer);

    //m_log << to_log;

    //std::clog << to_log;
}
