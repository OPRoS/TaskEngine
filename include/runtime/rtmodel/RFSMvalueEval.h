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
#include <map>
#include <sstream>
#include "common/GerrorHandler.h"
#include "common/Gdatatype.h"
#include "util/Utility.h"

class RFSMvalueEval 
{
private:
	bool			m_isNull;//초기화 여부를 검사

	unsigned int	m_type;		
	bool			m_isMinus;

	std::string		m_str; //"12"
	int				m_int;
	float			m_float;
	bool			m_bool;

	int				m_line;
	std::string		m_target;
	int				m_index; //함수의 파라메터로 사용될 때 순서를 지정할 때 사용
	
public:
	RFSMvalueEval(int line, std::string target);
	RFSMvalueEval(void);
	virtual ~RFSMvalueEval(void);

	void setLiteral(unsigned int type, std::string valuestr);
	void setMinus();
	void setType(unsigned int);

	unsigned int getType();
	
	std::string		getStrValue();
	int				getIntValue();
	float			getFloatValue();
	bool			getBoolValue();
	int				getLine();
	std::string		getTarget();
	int				getIndex();

	void setIndex(int);
	void setIntValue(int);
	void setFloatValue(float);
	void setBoolValue(bool);
	void setStrValue(std::string);
	void setLine(int);
	void setTarget(std::string);

	bool isNULL();

};