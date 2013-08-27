/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "run/control/ExeConnector.h"

ExeConnector::ExeConnector(RemoteCLI* svr, LogFile* lf, ExeTaskTreeNode* parent, std::string name, int nameLine, std::string blockID, std::string filename, std::map<int, int>* synchIDs, int callstmtLine, unsigned int tp)
:ExeTaskTreeNode(parent, svr, name, nameLine, blockID, filename, callstmtLine, tp)
{		
	m_logFile		= lf;
	m_SynchIdMap	= synchIDs;
	m_seq			= 1;
//	m_position = blockID;		
}

ExeConnector:: ~ExeConnector(void)
{
	stopRunning();

	for(unsigned int i=0; i<m_withList.size();i++){
		delete m_withList[i];
	}
	m_withList.clear();	

	//m_SynchIDs는 RFSMconnector가 있는 테이블에서 삭제된다.
}

void ExeConnector::setWithList(std::vector<ExeTransition*> wlist)
{
	for(unsigned int i=0; i<m_withList.size();i++) delete m_withList[i];
	m_withList.clear();	
	m_withList = wlist;
}

/*
void ExeConnector::removeWith(unsigned int index)
{
	bool deleted = false;
	for (unsigned int i=0; i<m_withList.size(); i++){
		if(i==index) {
			ExeTransition* with = m_withList.at(i);
			delete with;
			deleted = true;
			break;
		}
	}
	if(deleted) m_withList.erase(m_withList.begin()+index);
}
*/
int ExeConnector::execute(std::vector<ExeTaskTreeNode*> bhvlist, int conType)
{
	m_stop = false;
	setCompleted(false);
	std::string instName = getNodeName();
	instName.append(int2str(getCallStmtLine()));
	//m_synch = true;
	nodeStart();
	//ExeConnector는 쓰레드로 실행하지 않는다.
	this->create_thr(RThread::ATTACH, instName);
	//run();
	return 0;
}

void ExeConnector::setConType(int type)
{
	m_conType = type;
}

void ExeConnector::setJoinType(int type)
{
	m_joinType = type;
}

void ExeConnector::setCurActiveSeq(unsigned int seq)
{
	m_seq = seq;
}

int ExeConnector::getConType()
{
	return m_conType;
}

int ExeConnector::getJoinType()
{
	return m_joinType;
}

void ExeConnector::cbackBy_Thrfunc()
{	
	/*
	int conType = getConType();
	if(conType == SEQEXER){			
		
	}
	else if(conType == CONEXER){		
		for(unsigned int i=0; i<m_withList.size();i++){
			//Conexer -> Behavior
			if(m_withList[i]->getTransType()==ExeTransition::TO_BEHAVIOR){
				m_runner->link2Behavior(this, m_withList[i]);
			}
			//Conexer -> Conexer
			if(m_withList[i]->getTransType()==ExeTransition::TO_CONNECTOR){			
				m_runner->link2Connector(this, m_withList[i]);				
			}		
		}
	}
	*/
}

std::vector<ExeTransition*> ExeConnector::getWithList()
{
	return m_withList;
}

/*
std::map<int, int>*	ExeConnector::getSynchInfoMap()
{
	return m_SynchIdMap;
}
*/

/**
현재 노드에서 대기해야하는 SYNCH id의 수
*/
int ExeConnector::getSynchIDNum(int id)
{
	std::map<int, int>::iterator mapiter;
	mapiter = m_SynchIdMap->find(id);
	if(mapiter != m_SynchIdMap->end())
		return mapiter->second;
	else
		return 0;
}

unsigned int ExeConnector::getCurActiveSeq(void)
{
	return m_seq;
}

unsigned int ExeConnector::getNumOfRunBlocks(void)
{
	return m_withList.size();
}