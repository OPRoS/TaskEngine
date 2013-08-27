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

class RFSMObject
{
private:
	std::string m_className;

public:
	RFSMObject(std::string);
	virtual ~RFSMObject(void);

	
	std::string getClassName();

	virtual std::string toString()=0;
};

//단말 클래스만... 중간 자바 인터페이스역할의 추상클래스들은 필요 없음

const std::string CLS_RFSMvar 			= std::string("RFSMvar");
const std::string CLS_RFSMasmt 		= std::string("RFSMasmt");
const std::string CLS_RFSMcallAction   = std::string("RFSMcallAction");
const std::string CLS_RFSMcallFunc 	= std::string("RFSMcallFunc");
const std::string CLS_RFSMcallBehavior	= std::string("RFSMcallBehavior");
const std::string CLS_RFSMcallConnector= std::string("RFSMcallConnector");
const std::string CLS_RFSMgotoStmt 	= std::string("RFSMgotoStmt");
const std::string CLS_RFSMifStmt 		= std::string("RFSMifStmt");
const std::string CLS_RFSMblockStmt 	= std::string("RFSMblockStmt"); //block 내부가 일반적으로 실행
const std::string CLS_RFSMparStmt 		= std::string("RFSMparStmt");
const std::string CLS_RFSMseqStmt		= std::string("RFSMseqStmt");
const std::string CLS_RFSMwithStmt		= std::string("RFSMwithStmt");
const std::string CLS_RFSMsymbStmt 	= std::string("RFSMsymbStmt");

const std::string CLS_RFSMvariable 	= std::string("RFSMvariable");
const std::string CLS_RFSMliteral 		= std::string("RFSMliteral");
const std::string CLS_RFSMRelational	= std::string("RFSMRelational");
const std::string CLS_RFSMand 			= std::string("RFSMand");
const std::string CLS_RFSMor			= std::string("RFSMor");

const std::string CLS_RFSMadd 			= std::string("RFSMadd");
const std::string CLS_RFSMmulti 		= std::string("RFSMmulti");
const std::string CLS_RFSMcondValue 	= std::string("RFSMcondValue");
const std::string CLS_RFSMsign 		= std::string("RFSMsign");
const std::string CLS_RFSMenum 		= std::string("RFSMenum");
const std::string CLS_RFSMsystem 		= std::string("RFSMsystem");
const std::string CLS_RFSMsysWaitfor	= std::string("RFSMsysWaitfor");
const std::string CLS_RFSMsysSynch		= std::string("RFSMsysSynch");
