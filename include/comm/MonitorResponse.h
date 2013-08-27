#pragma once

#include <map>
#include <string>
#include <vector>
#include <time.h>
#include <iostream>
#include "common/TypeDef.h"
#include "thread/ThreadDef.h"
#include "util/Utility.h"

class MonitorResponse
{
private:	
	long			m_num; //일련으로 자동증가	
	unsigned int	m_type;
	std::string		m_data;
	pthread_mutex_t	m_mu;
	
public:
	MonitorResponse(void);
	virtual ~MonitorResponse(void);

	void init(unsigned int);
	
	void setDataType(unsigned int);
	std::string makeResponseMsg(std::string, int, int, std::string, int, std::string, int);
	std::string makeThreadMsg(std::string thid, std::string name, std::string pthid);
	void addFunc(std::string);
	void addFuncParam(std::string, RFSMvalueEval);
	void addFuncReturn(std::string, RFSMvalueEval);
	void addVariable(std::string, RFSMvalueEval);
	std::string makeVarAttribute(std::string, RFSMvalueEval);
	 
};


static unsigned int RES_DATA_in 	= 1;
static unsigned int RES_DATA_out	= 2;
static unsigned int RES_DATA_wvar 	= 3;
static unsigned int RES_DATA_gvar 	= 4;
static unsigned int RES_DATA_lvar 	= 5;
static unsigned int RES_DATA_func	= 6;
static unsigned int RES_DATA_param 	= 7;
static unsigned int RES_DATA_trans 	= 8;
static unsigned int RES_DATA_empty 	= 9;