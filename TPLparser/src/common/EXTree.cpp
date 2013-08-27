/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "common/EXTree.h"

EXTree::EXTree(Gtoken tok)
{
	m_id = 0;
	m_node = tok;
	m_refcount = 0;
	m_curIdx=0;
	m_id++;
}

EXTree::~EXTree()
{
	std::vector<EXTree*>::iterator it;  //= m_children->begin();

	for(it=m_children.begin(); it!=m_children.end(); ++it){
		EXTree* child = (*it);
		child->decreaseRef();
		if(child->getRefCount() <= 1){
			delete (*it);
		}
	}
	//m_children.clear();
}
void EXTree::rmChildrenPtr()
{
	std::vector<EXTree*>::iterator it;
	for(it=m_children.begin(); it!=m_children.end(); it++){
		EXTree* child = (*it);
		child->decreaseRef();
	}
	m_children.clear();
}

void EXTree::setNode(Gtoken node)
{
	m_node = node;
}

Gtoken EXTree::getNode()
{
	return m_node;
}

void EXTree::addChild(EXTree* child)
{
	child->increaseRef();
	m_children.push_back(child);
}

void EXTree::addChildAsFirst(EXTree* child)
{
	m_children.insert(m_children.begin(), child);
}

int EXTree::increaseRef()
{
	return ++m_refcount;
}

int EXTree::decreaseRef()
{
	return --m_refcount;
}

int EXTree::getRefCount()
{
	return m_refcount;
}

int EXTree::getChildrenNumber()
{
	return m_children.size();
}


bool EXTree::move2FirstChild()
{
	if(m_children.size()<1)
		return false;
	else{
		m_curIdx=0;	
		return true;
	}
}

bool EXTree::move2NextChild()
{
	if(m_curIdx < m_children.size()-1){
		m_curIdx++;
		return true;
	}
	else
		return false;
}

EXTree* EXTree::getCurrentChild()
{
	if(m_curIdx > m_children.size()-1)
		return NULL;
	else
		return m_children[m_curIdx];	
}

EXTree* EXTree::getFirstChild()
{
	if(move2FirstChild())
		return getCurrentChild();
	else
		return NULL;
}

EXTree* EXTree::getNextChild()
{
	if(move2NextChild())
		return getCurrentChild();
	else
		return NULL;
}

bool EXTree::delCurChild()
{
	bool result = false;
	EXTree* child = m_children[m_curIdx];
	child->decreaseRef();
	if(child->getRefCount() <= 1){		
		delete child;
		result =  true;
	}	
	std::vector<EXTree*>::iterator it = m_children.begin() + m_curIdx;
	m_children.erase(it);
	return result;
}

bool EXTree::cutChild(EXTree* c)
{
	if(m_children.size()>0){
		std::vector<EXTree*>::iterator it;
		for(it=m_children.begin(); it!=m_children.end(); it++){
			EXTree* child = (*it);
			if(child == c){
				child->decreaseRef();			
				m_children.erase(it);
				return true;
			}
		}
	}
	return false;
}

bool EXTree::cutCurrentChild()
{
	std::vector<EXTree*>::iterator it = m_children.begin() + m_curIdx;
	if(m_children.begin()<=it && it<=m_children.end()){
		EXTree* child = (*it);
		child->decreaseRef();	
		m_children.erase(it);
		return true;
	}
	return false;
}

bool EXTree::delDirectChildNode(EXTree* c)
{
	if(m_children.size()>0){
		std::vector<EXTree*>::iterator it;
		for(it=m_children.begin(); it!=m_children.end(); it++){
			EXTree* child = (*it);
			if(child == c){
				child->decreaseRef();
				child->rmChildrenPtr();
				delete child;
				return true;
			}
		}
	}
	return false;
}


void EXTree::print(void)
{
	printf("------------------< TREE >----------------------\n");
	print(0);
	printf("-------------------------------------------------\n");
}

void EXTree::print(int nbTabs)
{
	for (int i=0; i<nbTabs; i++) printf("|  ");

// m_node->getValue() 이부분은 ENode 클래스에 의존하는 것이다. 이부분을 동적으로 바인딩하게 해야하는데
// 아직 방법을 모르겠다.

	printf("|- nid%d", m_id);
	m_node.print();

	std::vector<EXTree*>::iterator it;
	for(it=m_children.begin(); it!=m_children.end(); it++){
		EXTree* child = (*it);
		child->print(nbTabs+1);
	}
}