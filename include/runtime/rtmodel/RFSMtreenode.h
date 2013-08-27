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
#include <algorithm>
#include "runtime/rtmodel/RFSMvalue.h"
#include "runtime/rtmodel/RFSMstate.h"
#include "runtime/rtmodel/RFSMtransition.h"
#include "runtime/rtmodel/RFSMnamedItem.h"
#include "runtime/rtmodel/RFSMBreakPoint.h"
#include "runtime/rtmodel/RFSMcst.h"
#include <map>
#include <set>

class RFSMtreenode : /*public RFSMBreakPoint,*/ public RFSMBlockInfo, public RFSMcst 
{
private:
	std::vector<RFSMvalue*>*	m_paramValues;	//a=3;와 같은 RFSMasmt, 실행할 때 값이 입력된다. calltask
	std::vector<std::string>	m_linkedNodes;   //이 node와 연결된 behavior/conexer들, {for dot}
	std::vector<std::string>	m_refTasks;		 //이 node를 사용하는 task들, loadw, unloadw
	std::string					m_blockID;

protected:
	bool				m_isMission;
	std::set<int>		m_synchIDs;     //RFSMconnector에서는 내용이 없다. RFSMbehavior에서 action block에 포함된 synch id
	std::set<int>		m_validSynchIDs;
	std::map<std::string, RFSMtreenode*> m_subNodes;

public:
	RFSMtreenode(std::string name, int nameLine, int endLine, std::string blockID, std::string file, bool);
	virtual ~RFSMtreenode(void);

	void setParamValues(std::vector<RFSMvalue*>*);
	void addRefTask(std::string);		//loadw, unloadw
	
	bool	isMission	(void);
	int		unRefTask	(std::string tname);
	bool	isReferred	(std::string tname);//loadw, unloadw	
	std::vector<std::string>	getRefTasks		(void);				//loadw, unloadw
	std::vector<std::string>	getLinkedNodes	(void);				//for dot
	std::map<std::string, RFSMtreenode*>	getSubNodes();

	void addLinkedNode		(std::string, int);	//for dot
	void addSubNode			(std::string, RFSMtreenode*);
	void addSynchID			(int);
	void setValidSynchIDs	(const std::set<int>);
	void collectSynIDs		(std::set<int>& ids);
};

const unsigned int BEHAVIOR	= 1;
const unsigned int CONNECTOR	= 2;