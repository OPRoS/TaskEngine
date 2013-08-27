/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMvalueEval.h"

RFSMvalueEval::RFSMvalueEval(int line, std::string target)
{
	m_isMinus	= false;
	m_isNull	= true;
	m_line		= line;
	m_target	= target;
	m_index		= -1;

	m_type = TYP_VOID; //void로 초기화
}

RFSMvalueEval::RFSMvalueEval()
{
	m_isMinus = false;
	m_isNull = true;
	m_index	= -1;
}

RFSMvalueEval:: ~RFSMvalueEval(void)
{
}

void RFSMvalueEval::setLiteral(unsigned int type, std::string valueStr)
{
	m_isNull = false;
	m_type= type;

	if(type == TYP_STRING)
		m_str = valueStr;
	if(type == TYP_INT)
		m_int = str2int(valueStr);
	if(type == TYP_FLOAT || type ==TYP_DOUBLE)
		m_float = str2float(valueStr);
	if(type == TYP_BOOL)
		m_bool = str2bool(valueStr);
}

unsigned int RFSMvalueEval::getType()
{
	return m_type;
}

void RFSMvalueEval::setType(unsigned int type)
{
	m_type = type;
}

void RFSMvalueEval::setMinus()
{
	m_isMinus = true;
}

int RFSMvalueEval::getIndex()
{
	return m_index;
}

void RFSMvalueEval::setIndex(int idx)
{
	m_index = idx;
}
std::string RFSMvalueEval::getStrValue()
{
	std::string result = "";
	if(m_isMinus)
		result = "-";
	if(m_type == TYP_INT)
		return result.append(int2str(m_int));
	if(m_type == TYP_FLOAT)
		return result.append(float2str(m_float));
	if(m_type == TYP_BOOL)
		return result.append(bool2str(m_bool));

	return m_str;
}


float RFSMvalueEval::getFloatValue()
{
	float rtn = m_float;

	//int인데 float를 호출하면 float로 casting한다.
	if(m_type == TYP_INT)
		rtn = (float)m_int;

	if(m_isMinus)
		return (-1)*rtn;
	return rtn;
}

int RFSMvalueEval::getIntValue()
{
	int rtn = m_int;

	//타입이 float인데 정수를 원하면 int로 casting에서 전달
	if(m_type == TYP_FLOAT)
		rtn = (int)m_float;

	if(m_isMinus) return (-1)* rtn;
	else return rtn;
}

bool RFSMvalueEval::getBoolValue()
{
	if(m_isNull) return false;

	bool rtn = m_bool;

	if(m_type == TYP_INT)
		rtn = (bool)m_int;
	if(m_type == TYP_FLOAT)
		rtn = (bool)m_float;

	return rtn;
}

int RFSMvalueEval::getLine()
{
	return m_line;
}

std::string RFSMvalueEval::getTarget()
{
	return m_target;
}

void RFSMvalueEval::setIntValue(int intValue)
{
	m_isNull = false;
	m_type=TYP_INT;
	m_int = intValue;
}

void RFSMvalueEval::setFloatValue(float floatValue)
{
	m_isNull = false;
	m_type=TYP_FLOAT;
	m_float = floatValue;
}

void RFSMvalueEval::setBoolValue(bool boolValue)
{
	m_isNull = false;
	m_type=TYP_BOOL;
	m_bool = boolValue;
}

void RFSMvalueEval::setStrValue(std::string str)
{
	m_isNull = false;
	m_type=TYP_STRING;
	m_str =str;
}
void RFSMvalueEval::setLine(int line)
{
	m_line = line;
}

void RFSMvalueEval::setTarget(std::string target)
{
	m_target = target;
}

bool RFSMvalueEval::isNULL()
{
	return m_isNull;
}
