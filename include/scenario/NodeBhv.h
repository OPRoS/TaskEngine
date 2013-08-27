/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Scenario 
** Date:        2011.08.15
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "scenario/SBehavior.h"
#include "scenario/NodeBS.h"

class NodeBhv : public NodeBS
{
private:	
	SBehavior*		m_bhv;
	std::string		m_bhvRef;
	NodeBS*			m_sub;
	std::string		m_subMonOp; //~, ^, *
	std::string		m_subMonEvt;
	bool			m_isCycled; //하위노드에서 현재노드를 참조하는 경우, 사이클 발생

public:
	NodeBhv(int);
	virtual ~NodeBhv(void);

	void setSBehavior	(SBehavior*);
	void setBhvRef		(std::string);
	void setSub			(NodeBS*);
	void setSubMonOp	(std::string);
	void setSubMonEvt	(std::string);
	void setAsCycled	(void);
	

	SBehavior*	getSBehavior	(void);
	std::string	getBhvRef		(void);
	NodeBS*		getSubNode		(void);
	std::string	getSubMonOp		(void);
	std::string getSubMonEvt	(void);
	bool		isCycled		(void);
};