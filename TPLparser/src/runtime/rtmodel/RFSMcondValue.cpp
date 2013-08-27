/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMcondValue.h"

RFSMcondValue::RFSMcondValue(RFSMboolable* cond, RFSMvalue* tval, RFSMvalue* fval, std::string file, int line)
	:RFSMvalue(tval->getBlockID(), tval->getBlockName(), file, line)
{
	m_condSymbol = cond;
	m_condT_Rvalue = tval;
	m_condF_Rvalue = fval;
}

RFSMcondValue:: ~RFSMcondValue(void)
{
	if(m_condSymbol != 0) delete m_condSymbol;
	if(m_condT_Rvalue != 0 ) delete m_condT_Rvalue;
	if(m_condF_Rvalue != 0) delete m_condF_Rvalue;
}

RFSMvalue* RFSMcondValue::getTrueValue()
{
	return m_condT_Rvalue;
}

RFSMvalue* RFSMcondValue::getFalseValue()
{
	return m_condF_Rvalue;
}

RFSMboolable* RFSMcondValue::getCondition()
{
	return m_condSymbol;
}

/**
* True�� ���� False�� �� � ���� ������ �𸣱� ������
* �� ū���� Ÿ������ ����Ѵ�.
*/
unsigned int RFSMcondValue::getType()
{
	//�Ѵ� string�̰ų� �Ѵ� bool, ���� Ÿ��
	if(m_condT_Rvalue->getType() == m_condF_Rvalue->getType())
		return m_condT_Rvalue->getType();

	//���� Ÿ���� ��� ���̰� �� Ÿ��
	return computationType(m_condT_Rvalue->getType(), m_condF_Rvalue->getType());
}

std::string RFSMcondValue::toString()
{
	std::string rtn = "";
	std::string str = RFSMboolable::toString(m_condSymbol);
	std::string str1 = RFSMvalue::toString(m_condT_Rvalue);
	std::string str2 = RFSMvalue::toString(m_condF_Rvalue);
	rtn.append(str).append(" ").append(str1).append(" ").append(str2);
	return rtn;
}
