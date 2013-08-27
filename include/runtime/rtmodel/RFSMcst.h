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
#include "runtime/rtmodel/RFSMvalue.h"
#include "runtime/rtmodel/RFSMnamedItem.h"
#include "runtime/rtmodel/RFSMstmt.h"
#include "runtime/rtmodel/RFSMasmt.h"

class RFSMcst : public RFSMnamedItem
{
private:
	std::vector<RFSMstmt*>*	m_const;
	std::vector<RFSMstmt*>*	m_dest;
	std::vector<RFSMstmt*>*	m_varAsmts;
	
	int m_nameLine;
	int m_EOLconst;
	int m_EOLdest;
	
	//std::string			m_blockID;

public:
	RFSMcst(std::string name, int nameLine, std::string blockID);
	virtual ~RFSMcst(void);

	void setConstruct	(std::vector<RFSMstmt*>*);
	void setDestruct	(std::vector<RFSMstmt*>*);
	void addVarAsmt		(RFSMstmt*);			//var:블록에서 선언과 동시에 할당을 하는 문장저장
												//즉, 할당문(RFSMasmt)과 변수선언(RFSMvar, 이건 디버깅 때문에 필요)
	//std::string				getBlockID();	
	std::vector<RFSMstmt*>*	getVarAsmts();
	std::vector<RFSMstmt*>*	getConstruct();
	std::vector<RFSMstmt*>*	getDestruct();

	void setEOLconst	(int);
	void setEOLdest		(int);
	int	 getNameLine	(void);
	int  getEOLconst	(void);
	int  getEOLdest		(void);

};
