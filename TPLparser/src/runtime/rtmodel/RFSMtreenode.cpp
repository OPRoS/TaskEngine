/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "runtime/rtmodel/RFSMtreenode.h"

RFSMtreenode::RFSMtreenode(std::string name, int nameLine, int endLine, std::string blockID, std::string file, bool isMission)
	:RFSMcst(name, nameLine, blockID), 
	 /*RFSMBreakPoint(file)*/
	 RFSMBlockInfo(blockID, name, file, endLine)
{
	//m_blockID = blockID;
	m_paramValues = NULL;
	m_isMission = isMission;
	//m_const = NULL;
	//m_dest = NULL;
	//m_varAsmts	=  NULL;
}

RFSMtreenode:: ~RFSMtreenode(void)
{
	if(m_paramValues != NULL){
		/*while(m_paramValues->getSize()>0 && m_paramValues->moveHead()){
			RFSMvalue* obj = m_paramValues->getCurObject();
			delete obj;
			m_paramValues->delHead();
		}*/
		for(unsigned int i=0; i<m_paramValues->size();i++){
			delete (*m_paramValues)[i];
		}
		delete m_paramValues;
	}
	/*
	if(m_const != NULL){
		while(m_const->getSize()>0 && m_const->moveHead()){
			RFSMstmt* obj = m_const->getCurObject();
			delete obj;
			m_const->delHead();
		}
		delete m_const;
	}
	
	if(m_dest != NULL){
		while(m_dest->getSize()>0 && m_dest->moveHead()){
			RFSMstmt* obj = m_dest->getCurObject();
			delete obj;
			m_dest->delHead();
		}
		delete m_dest;
	}

	if(m_varAsmts != NULL){
		while(m_varAsmts->getSize()>0 && m_varAsmts->moveHead()){
			RFSMstmt* obj = m_varAsmts->getCurObject();
			delete obj;
			m_varAsmts->delHead();
		}
		delete m_varAsmts;
	}
	*/
	//m_linkedNodes.addTail()을 하면
	//EListElement<T> *newElement = new EListElement<T>(obj);에서 T가 포인터가 아니지만
	//newElement가 new이기 때문에 삭제해야 한다.
	
	//while(m_linkedNodes.getSize()>0 && m_linkedNodes.moveHead()){	
	//	m_linkedNodes.delHead();
	//}
		
	//while(m_refTasks.getSize()>0 && m_refTasks.moveHead()){	
	//	m_refTasks.delHead();
	//}

	m_linkedNodes.clear();
	m_refTasks.clear();
}

void RFSMtreenode::setParamValues(std::vector<RFSMvalue*>* pvalues)
{
	if(m_paramValues != NULL){
		for(unsigned int i=0; i<m_paramValues->size();i++){
			delete (*m_paramValues)[i];
		}
		delete m_paramValues;
	}

	m_paramValues = pvalues;
}

void RFSMtreenode::addLinkedNode(std::string linkedBhv, int type)
{
	//type 1: behavior, type 2: connector
	m_linkedNodes.push_back(linkedBhv);
}

void RFSMtreenode::addSubNode(std::string name, RFSMtreenode* obj)
{
	m_subNodes.insert(std::pair<std::string, RFSMtreenode*>(name, obj));
}

void RFSMtreenode::addSynchID(int id)
{
	m_synchIDs.insert(id);
}


void RFSMtreenode::setValidSynchIDs(const std::set<int> vids)
{
	if(vids.size()>0) m_validSynchIDs = vids;
}


void RFSMtreenode::collectSynIDs(std::set<int>& ids)
{
	//1. 자기의 synch id 삽입
	std::set<int>::iterator myids_iter;
	for(myids_iter=m_synchIDs.begin(); myids_iter!=m_synchIDs.end(); myids_iter++){
		ids.insert(*myids_iter);
	}

	if(m_subNodes.size()>0){
		std::map<std::string, RFSMtreenode*>::iterator it;
		for(it=m_subNodes.begin(); it!=m_subNodes.end(); it++){
			RFSMtreenode* node = it->second;
			node->collectSynIDs(ids);
		}
	}
}

void RFSMtreenode::addRefTask(std::string tname)
{
	bool exist = false;
	for(unsigned int i=0; i<m_refTasks.size();i++){
		if(m_refTasks[i]==tname){
			exist = true;
			break;
		}
	}
	if(!exist) m_refTasks.push_back(tname);
}

std::vector<std::string> RFSMtreenode::getLinkedNodes()
{
	return m_linkedNodes;
}

std::map<std::string, RFSMtreenode*> RFSMtreenode::getSubNodes()
{
	return m_subNodes;
}

std::vector<std::string> RFSMtreenode::getRefTasks()
{
	return m_refTasks;
}

bool RFSMtreenode::isMission()
{
	return m_isMission;
}

bool RFSMtreenode::isReferred(std::string workerName)
{
	/*if(m_refTasks.getSize()>0 && m_refTasks.moveHead()){
		do{
			std::string wName = m_refTasks.getCurObject();
			if(workerName.compare(workerName)==0)
				return true;

		}
		while(m_refTasks.moveNext());
	}*/

	if(m_refTasks.size()>0){
		for(unsigned int i=0; i<m_refTasks.size();i++){
			std::string wName = m_refTasks[i];
			if(workerName.compare(workerName)==0)
				return true;
		}
	}
	return false;
}

int RFSMtreenode::unRefTask(std::string workerName)
{
	if(m_refTasks.size()>0){
		for(unsigned int i=0; i<m_refTasks.size();i++){
			std::string wName = m_refTasks[i];
			if(wName.compare(workerName)==0){
				m_refTasks.erase(m_refTasks.begin()+i);
				break;
			}
		}
	}
	return m_refTasks.size();
}

