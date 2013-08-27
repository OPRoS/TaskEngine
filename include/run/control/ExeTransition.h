/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include <map>
#include <string>
#include "common/TypeDef.h"
#include "runtime/rtmodel/RFSMnamedItem.h"

class ExeTransition: public RFSMnamedItem
{
private:	
	int				m_transType;		//connector(connexer|seqexer), behavior, state
	ValueMap*		m_params;			//connector behavior
	std::string		m_nextTarget;		//~>연산자 이후에 천이 state
	int				m_stmtLine;			//Mission노드를 식별하는 key로 활용하기 위함, (-1은 ~> 상태 천이, 이 경우는 미션이 아니므로 상관없음 )
	int				m_period;

public:
	ExeTransition(int, std::string, std::string, int, int);
	virtual ~ExeTransition(void);

	void	setTransType	(int);
	void	setParameters	(ValueMap*);

	int			getTransType		(void);
	ValueMap*	getParameters		(void);
	std::string	getNextTargetState	(void);
	int			getStmtLine			(void);
	int			getPeriod			(void);
	
};

const unsigned int TO_CONNECTOR	= 1;
const unsigned int TO_BEHAVIOR	= 3;
const unsigned int TO_STATE		= 4;
