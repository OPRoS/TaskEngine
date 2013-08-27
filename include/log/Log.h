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
//ASSERT는 If문을 사용하지 않고 바고 LOG처럼 사용할 수 있다는 점이 위의 로그와 차이
//exp에 비교,관계표현문
//실행시에는 assert를 무시하기 위해, ignoreAlways = true;이미 assert가 호출되지 않는다.
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
//##은 리스트 즉, ... 부분이 안 올경우 ,(쉼표)가 뒤에 붙어서 에러남으로 이를 제거
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
* 이 클래스는 로그를 할 대상에 대한
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

	static const unsigned int LOG_LEVEL_DEBUG	= 0; //디버깅 모드에서 검증하는 부분
	static const unsigned int LOG_LEVEL_TRACE	= 1; //현재 상태 보여줌
	static const unsigned int LOG_LEVEL_INFO	= 2; //권고안
	static const unsigned int LOG_LEVEL_WARN	= 3; //이건 이래야하는데..
	static const unsigned int LOG_LEVEL_ERROR	= 4; //에러발생
	static const unsigned int LOG_LEVEL_FATAL	= 5; //복구 불능, 시스템정지
};
