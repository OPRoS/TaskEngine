/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Scenario 
** Date:        2011.08.15
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include <string>

class NodeTrans
{
private:
	std::string		m_cond;
	std::string		m_sourceName;
	std::string		m_targetName;
	unsigned int	m_TargetType;
	std::string		m_nextState;
	std::string		m_monPrefix; //source가 예외처리를 포함하는 state인 경우에만 값을 가짐

public:
	NodeTrans(void);
	virtual ~NodeTrans(void);

	void setTransSource		(std::string);
	void setCond			(std::string);
	void setTransTarget		(unsigned int, std::string);
	void setStateAfterExpand(std::string);
	void setMonPrefix		(std::string);
	

	unsigned int	getTransType		(void);
	std::string		getTransTarget		(void);
	std::string		getTransSource		(void);
	std::string		getStateAfterExpand	(void);
	std::string		getCond				(void);
	std::string		getMonPrefix		(void);
};

const static int EXPAND_ = 1;
const static int MOVETO_ = 2;