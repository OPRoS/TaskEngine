/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include <iostream>
#include <assert.h>
#include <stdio.h>
#include <stdarg.h>

/*
#define LOG_DEBUG(log, msg)\
{ \
	if ((log).isEnabled((Log::LOG_LEVEL_DEBUG))) { \
		(log).printMsg("DEBUG", __FUNCTION__, __FILE__, __LINE__, (msg)); \
    } \
}
#define LOG_TRACE(log, msg)\
{ \
	if ((log).isEnabled((Log::LOG_LEVEL_TRACE))) { \
		(log).printMsg("DEBUG", __FUNCTION__, __FILE__, __LINE__, (msg)); \
    } \
}
#define LOG_INFO(log, msg)\
{ \
	if ((log).isEnabled((Log::LOG_LEVEL_INFO))) { \
		(log).printMsg("INFO", __FUNCTION__, __FILE__, __LINE__, (msg)); \
    } \
}
#define LOG_WARN(log, msg)\
{ \
	if ((log).isEnabled((Log::LOG_LEVEL_WARN)) { \
		(log).printMsg("WARN", __FUNCTION__, __FILE__, __LINE__, (msg)); \
    } \
}

#define LOG_ERROR(log, msg)\
{ \
	if ((log).isEnabled((Log::LOG_LEVEL_ERROR))) { \
		(log).printMsg("ERROR", __FUNCTION__, __FILE__, __LINE__, (msg)); \
    } \
}
#define LOG_FATAL(log, msg)\
{ \
	if ((log).isEnabled((Log::LOG_LEVEL_FATAL))) { \
		(log).printMsg("FATAL", __FUNCTION__, __FILE__, __LINE__, (msg)); \
    } \
}
*/
//ASSERT�� If���� ������� �ʰ� �ٰ� LOGó�� ����� �� �ִٴ� ���� ���� �α׿� ����
//exp�� ��,����ǥ����
//����ÿ��� assert�� �����ϱ� ����, ignoreAlways = true;�̹� assert�� ȣ����� �ʴ´�.
/*
#define ASSERT(log, exp, msg)\
{\
	static bool ignoreAlwasys = false;\
	if(!ignoreAlwasys){\
		if((log).assertR((int)(exp), __FILE__, __LINE__, (msg)))\
		{ _asm { int 3 } }\
	}\
}
*/
//variadic macro
//##�� ����Ʈ ��, ... �κ��� �� �ð�� ,(��ǥ)�� �ڿ� �پ ���������� �̸� ����
#define Variadic_macro_test(fmt, ...) err_func(__FILE__, __LINE__, fmt, ##__VA_ARGS__)


#define LOG_DEBUG(log, msg, ...)\
{ \
	if ((log).isEnabled((Log::LOG_LEVEL_DEBUG))) { \
		(log).printHead("DEBUG", __FUNCTION__, __FILE__, __LINE__); \
		(log)((msg), ##__VA_ARGS__);\
    } \
}
#define LOG_TRACE(log, msg, ...)\
{ \
	if ((log).isEnabled((Log::LOG_LEVEL_TRACE))) { \
		(log).printHead("TRACE"); \
		(log)((msg), ##__VA_ARGS__);\
    } \
}
#define LOG_INFO(log, msg, ...)\
{ \
	if ((log).isEnabled((Log::LOG_LEVEL_INFO))) { \
		(log).printHead("INFO"); \
		(log)((msg), ##__VA_ARGS__);\
    } \
}
#define LOG_WARN(log, msg, ...)\
{ \
	if ((log).isEnabled((Log::LOG_LEVEL_WARN))) { \
		(log).printHead("WARN"); \
		(log)((msg), ##__VA_ARGS__);\
    } \
}
#define LOG_ERROR(log, msg, ...)\
{ \
	if ((log).isEnabled((Log::LOG_LEVEL_ERROR))) { \
		(log).printHead("ERROR"); \
		(log)((msg), ##__VA_ARGS__);\
    } \
}
#define LOG_FATAL(log, msg, ...)\
{ \
	if ((log).isEnabled((Log::LOG_LEVEL_FATAL))) { \
		(log).printHead("FATAL", __FUNCTION__, __FILE__, __LINE__); \
		(log)((msg), ##__VA_ARGS__);\
    } \
}

#define FLOG_TRACE(log, msg, ...)\
{ \
	if((log)!=NULL){\
		if ((log)->isEnabled((Log::LOG_LEVEL_TRACE))) { \
			(log)->printHead("TRACE", __FUNCTION__, __FILE__, __LINE__); \
			(log)->writeMsg((msg), ##__VA_ARGS__);\
		} \
	}\
}

#define FLOG_ERROR(log, msg, ...)\
{ \
	if((log)!=NULL){\
		if ((log)->isEnabled((Log::LOG_LEVEL_ERROR))) { \
			(log)->printHead("ERROR", __FUNCTION__, __FILE__, __LINE__); \
			(log)->writeMsg((msg), ##__VA_ARGS__);\
		} \
	}\
}

/**
* �� Ŭ������ �α׸� �� ��� ����
*
*/
class Log
{
private:
	std::string			m_name;
	unsigned int		m_level;
	std::ostream*		m_ostream;

	bool				m_levelStatus[6];

public:
	Log();
	Log(char* name, std::ostream*);
	virtual ~Log(void);

	void setName(std::string);
	void setLevel(unsigned int level, bool activated);
	void printHead(const char*);
	void printHead(const char*, const char*, const char*, long);
	bool assertR(bool, char*, long, char*);
	bool isEnabled(unsigned int);

	void operator()(const char* input, ...);

	static const unsigned int LOG_LEVEL_DEBUG	= 0; //����� ��忡�� �����ϴ� �κ�
	static const unsigned int LOG_LEVEL_TRACE	= 1; //���� ���� ������
	static const unsigned int LOG_LEVEL_INFO	= 2; //�ǰ��
	static const unsigned int LOG_LEVEL_WARN	= 3; //�̰� �̷����ϴµ�..
	static const unsigned int LOG_LEVEL_ERROR	= 4; //�����߻�
	static const unsigned int LOG_LEVEL_FATAL	= 5; //���� �Ҵ�, �ý�������
};
