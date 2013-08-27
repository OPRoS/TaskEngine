/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include <vector>
#include "stxmodel/Gtoken.h"

class EXTree
{
private:	
	int						m_id;
	unsigned int			m_refcount;
	unsigned int			m_curIdx; 
	Gtoken					m_node;			
	std::vector<EXTree*>	m_children;
	
public:
	EXTree(Gtoken node);
	virtual ~EXTree(void);
	
	void addChild			(EXTree*);	
	void addChildAsFirst	(EXTree*);
	void setNode			(Gtoken);
	void rmChildrenPtr		(void);	
	//void delChild(EXTree*);

	Gtoken	getNode			(void);
	int		increaseRef		(void);
	int		decreaseRef		(void);
	int		getRefCount		(void);
	int		getChildrenNumber(void);

	bool	move2FirstChild	(void);
	bool	move2NextChild	(void);
	EXTree* getCurrentChild	(void);
	EXTree* getFirstChild	(void);
	EXTree* getNextChild	(void); //move2NextChild and getCurChild
	bool	delCurChild		(void);
	bool	cutChild		(EXTree*);
	bool	cutCurrentChild	(void);
	bool	delDirectChildNode(EXTree* c);

	void	print(void);
	void	print(int nbTabs);
};

