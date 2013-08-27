/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Scenario 
** Date:        2011.08.15
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "scenario/SEvent.h"

class NodeBS
{
private:	
	//SEvent*		m_cond;
	std::string		m_evtRef;
	int				m_id;
	//std::string		m_uname;
	std::string		m_state;  //NodeBS를 호출하는 State이름
	std::string		m_prefix; //behavior sequence에서 앞에 path

	std::vector<std::string> m_expandlist;

public:
	NodeBS(int);
	virtual ~NodeBS(void);

	//void	setSEvent		(SEvent*);
	void	setEvtRef		(std::string);
	//void	setUname		(std::string);
	void	addExpandStr	(std::string);
	void	setState		(std::string);
	void	setPrefix		(std::string);
	
	//SEvent*		getSEvent	(void);
	std::string		getEvtRef	(void);	
	int				getNodeId	(void);
	//std::string		getUname	(void);
	std::string		getState	(void);
	std::string		getPrefix	(void);

};
