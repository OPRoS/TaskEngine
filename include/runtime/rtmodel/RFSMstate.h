/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once
#include "runtime/rtmodel/RFSMtransition.h"
#include "runtime/rtmodel/RFSMstmt.h"
#include "runtime/rtmodel/RFSMasmt.h"
#include "runtime/rtmodel/RFSMnamedItem.h"
#include "runtime/rtmodel/RFSMBreakPoint.h"
#include "runtime/rtmodel/RFSMcst.h"
#include "common/Gdatatype.h"
#include <string>
#include <vector>

class RFSMstate : public RFSMcst, public RFSMBlockInfo /*, public RFSMBreakPoint */
{
private:
	int	m_mod; //initial, goal
	std::vector<RFSMstmt*>*		m_transition;
	//EList<std::string>		m_linkedStates;	//for dot
	//EList<std::string>		m_linkedTasks;	//for dot
	std::vector<std::string>	m_linkedStates;	
	std::vector<std::string>	m_linkedTasks;	
	//EList<RFSMstmt*>*			m_dos;
	std::vector<RFSMstmt*>*		m_dos;

	int m_EOL_trans;
	int m_EOL_stay;
	//std::string m_curInvokedTask; //시멘틱스 검사에서 if문 내에 goto문의 bhv 호출이 무조건 하나만 와야한다.

public:
	RFSMstate(std::string path, std::string name, int nameline, std::string blockid, std::string file, int eol);
	virtual ~RFSMstate(void);

	void setModifier		(int);
	void setTransition		(std::vector<RFSMstmt*>*);
	void setDostmts			(std::vector<RFSMstmt*>*);
	void setCurInvokedTask	(std::string);
	void addLinkedstate		(std::string state); //for dot
	void addLinkedtask		(std::string bhv);	//for dot

	std::vector<std::string>*	getLinkedStates	(void);	//for dot
	std::vector<std::string>*	getLinkedTasks	(void);	//for dot
	std::vector<RFSMstmt*>*		getTransition	(void);
	std::vector<RFSMstmt*>*		getDostmts		(void);

	int		getModifier	(void);
	bool	isGoalState	(void);
	int		getEOLtrans	(void);
	int		getEOLstay	(void);
	void	setEOLtrans	(int);
	void	setEOLstay	(int);
};
