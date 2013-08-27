/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/GstmtIf.h"

GstmtIf::GstmtIf(std::string file, int eol, std::string blockname, std::string blockpath): Gstmt(STMT_if, file, eol, blockname, blockpath)
{
	m_ifAction		= NULL;
	m_elseAction	= NULL;
	m_cond			= NULL;
}

GstmtIf:: ~GstmtIf(void)
{
	delete m_ifAction;
	delete m_elseAction;
	delete m_cond;
}

void GstmtIf::setIfCondition(Gcondition *cond)
{
	if(m_cond != NULL)
		delete m_cond;
	m_cond = cond;
}

void GstmtIf::setIfAction(Gstmt* ifAction)
{
	if(m_ifAction != NULL)
		delete m_ifAction;

	m_ifAction = ifAction;
}

void GstmtIf::setElseAction(Gstmt* elseAction)
{
	if(m_elseAction != NULL)
		delete m_elseAction;

	m_elseAction = elseAction;
}

Gcondition* GstmtIf::getIfCond()
{
	return m_cond;
}

Gstmt* GstmtIf::getIfAction()
{
	if(m_ifAction != NULL)
		return m_ifAction;
	else
		return 0;
}

Gstmt* GstmtIf::getElse()
{
	if(m_elseAction != NULL)
		return m_elseAction;
	else
		return 0;
}

void GstmtIf::print(unsigned int indent)
{
	titlePrint(indent, std::string("IF"));

	if(m_cond != NULL){
		titlePrint(indent+1, std::string("COND"));
		m_cond->print(indent +2);
	}
	//Gcondition::print(indent+1);

	if(m_ifAction != NULL){
		titlePrint(indent+1, std::string("IFACTION"));
		m_ifAction->print(indent +2);
	}
	if(m_elseAction != NULL){
		titlePrint(indent+1, std::string("ESLEACTION"));
		m_elseAction->print(indent +2);
	}
}

/*
void GstmtIf::toDNF()
{
	bool changed = false;
	ETree<Gtoken>* eTree = m_cond->getExprTree();

	if(eTree->moveFirstChild()){
		do{
			ETree<Gtoken>* child = eTree->getCurrentChild();
			changed = iterDNF(eTree, child);
			if(changed) break;
		}
		while(eTree->moveNextChild());
	}

	if(changed) toDNF();
}

bool GstmtIf::iterDNF(ETree<Gtoken>* parent, ETree<Gtoken>* curTreeNode)
{
	//tree의 내용
	Gtoken token = curTreeNode->getNode();
	unsigned int type = token.getType();

	//1. NOT을 안쪽으로 집어 넣어야 한다. eg. !(!A)= A, !(!A and B) = (A or !B), !(A>B) = (A<=B)
	if(type == NOT){
		ETree<Gtoken>* child = curTreeNode->getFirstChild(); //NOT은 단항 연산자
		Gtoken cNode = child->getNode();

		//1.1 NOT이 2번 연속으로 나오는 경우 : NOT을 생략
		if(cNode.getType() == NOT){
			ETree<Gtoken>* child2 = child->getFirstChild();
			curTreeNode->delDirectChildNode(child);
			parent->delDirectChildNode(curTreeNode);
			parent->addChild(child2);
			return true;
		}
		//1.2 NOT다음에 AND나 OR가 있으면 AND나 OR를 서로 바꾸고 NOT을 안쪽으로 넣는다.
		else if(cNode.getType() == AND || cNode.getType() == OR){

			Gtoken orandToken;
			if(cNode.getType() == AND)orandToken = Gtoken(cNode.getfName(), cNode.getBlockPath(), "OR",-1, OR);
			if(cNode.getType() == OR)orandToken = Gtoken(cNode.getfName(), cNode.getBlockPath(), "AND",-1, AND);
			ETree<Gtoken>* orandNode = new ETree<Gtoken>(orandToken);

			if(child->moveFirstChild()){
				do{
					Gtoken notToken = Gtoken(cNode.getfName(), cNode.getBlockPath(), "NOT",-1, NOT);
					ETree<Gtoken>* notNode = new ETree<Gtoken>(notToken);
					notNode->addChild(child->getCurrentChild());
					orandNode->addChild(notNode);
				}
				while(child->moveNextChild());
			}

			curTreeNode->delDirectChildNode(child);  //And, OR 삭제
			parent->delDirectChildNode(curTreeNode); //not 삭제

			parent->addChild(orandNode);
			return true;
		}
		//1.3 NOT다음에 BOOL expr이 오면 NOT을 없애고 BOOL 연산자를 반대로 변환한다.
		else if(cNode.getType() == BOOL){
			ETree<Gtoken>* child3 = child->getFirstChild()->getFirstChild(); //첫번째에 연산자가 있다.
			Gtoken boolop = child3->getNode();
			std::string op = boolop.getData();

			if(op.compare("==")==0){
				op = std::string("!=");
			}
			else if(op.compare("!=")==0){
				op = std::string("==");
			}
			else if(op.compare("<")==0){
				op = std::string(">=");
			}
			else if(op.compare(">")==0){
				op = std::string("<=");
			}
			else if(op.compare("<=")==0){
				op = std::string(">=");
			}
			else if(op.compare(">=")==0){
				op = std::string("<");
			}
			boolop.setData(op);
			child3->setNode(boolop);

			parent->delDirectChildNode(curTreeNode);
			parent->addChild(child);
			return true;
		}
	}

	//curTreeNode 가 AND 또는 OR이면
	else if(type == AND||type == OR){

		unsigned int cNum = curTreeNode->getChildrenNumber();

		//자식이 하나이고 그 자식이 AND나 OR이면 현재의 AND나 OR를 없앤다.(DNF생성 중에 만들어진 이상한 형태들)
		//A and (B or C) -> (A and B) or (A and C) 이다.
		//사실(A and B) or (A and C) 형태는 (A and B) or (A and C) and nothing
		//A and (B or C) and D경우를 생각해 보라...
		if(parent->getChildrenNumber() == 1 && cNum ==1){
			ETree<Gtoken>* child = curTreeNode->getFirstChild();
			parent->delDirectChildNode(curTreeNode);
			parent->addChild(child);
			return true;
		}

		//같은 AND나 OR가 반복해서 나오면 중복을 지우고 자식을 부모의 자식으로 붙인다.
		unsigned int pType =  parent->getNode().getType();
		if((pType==AND && type == AND) || (pType == OR && type ==OR)){
			if(curTreeNode->moveFirstChild()){
				do{
					ETree<Gtoken>* c = curTreeNode->getCurrentChild();
					parent->addChild(c);
				}
				while(curTreeNode->moveNextChild());
				parent->delDirectChildNode(curTreeNode);
				return true;
			}
		}


		//바로 아래에 OR가 있는지 봐야 한다. (A or B) and C --> (A and C) or (B and C)
		if(cNum>1 && type == AND){
			bool hasOR =false;
			//child>1이고 OR가 child이면 이 child를 첫번째로 옮긴다.
			//A and B and (C or D) --> (C or D) and A and B
			if(curTreeNode->moveFirstChild()){
				do{
					ETree<Gtoken>* child = curTreeNode->getCurrentChild();
					Gtoken childNode = child->getNode();
					if(childNode.getType()==OR){
						curTreeNode->cutCurChild();
						curTreeNode->addChildAsFirst(child);
						hasOR = true;
						break;
					}
				}
				while(curTreeNode->moveNextChild());
			}
			if(hasOR){
				// (C or D) and A and B  = > (C and A) or (D and A) and B
				ETree<Gtoken> *orp = curTreeNode->getFirstChild(); //앞에서 옮겨서 처음이 or
				ETree<Gtoken> *second = curTreeNode->getNextChild();

				Gtoken newOrToken = Gtoken(orp->getNode().getfName(), orp->getNode().getBlockPath(), orp->getNode().getData(),orp->getNode().getLine(),orp->getNode().getType());
				ETree<Gtoken> *newOr = new ETree<Gtoken>(newOrToken);

				if(orp->moveFirstChild()){
					do{
						Gtoken andp = Gtoken(orp->getNode().getfName(), orp->getNode().getBlockPath(), "AND",-1, AND);
						ETree<Gtoken>* andNode = new ETree<Gtoken>(andp);
						andNode->addChild(orp->getCurrentChild());
						andNode->addChild(second);
						newOr->addChild(andNode);
					}
					while(orp->moveNextChild());

				}
				//curTreeNode->clearCurChild();
				curTreeNode->cutChild(second);
				curTreeNode->delDirectChildNode(orp);
				curTreeNode->addChild(newOr);
				return true;
			}

			//바로 아래에 NOT이 있는 경우
			if(curTreeNode->moveFirstChild()){
				do{
					ETree<Gtoken>* c = curTreeNode->getCurrentChild();
					Gtoken childNode = c->getNode();
					if(childNode.getType()==NOT){
						return iterDNF(curTreeNode, c);
					}
				}
				while(curTreeNode->moveNextChild());
			}

		}

	}

	//그냥 트리 아래로
	if(curTreeNode->moveFirstChild()){
		do{
			ETree<Gtoken>* c = curTreeNode->getCurrentChild();
			iterDNF(curTreeNode, c);
		}
		while(curTreeNode->moveNextChild());
	}
	else{
		//leafnode
	}


	return false;
}
*/

