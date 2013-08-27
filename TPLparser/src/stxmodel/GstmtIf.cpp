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
	//tree�� ����
	Gtoken token = curTreeNode->getNode();
	unsigned int type = token.getType();

	//1. NOT�� �������� ���� �־�� �Ѵ�. eg. !(!A)= A, !(!A and B) = (A or !B), !(A>B) = (A<=B)
	if(type == NOT){
		ETree<Gtoken>* child = curTreeNode->getFirstChild(); //NOT�� ���� ������
		Gtoken cNode = child->getNode();

		//1.1 NOT�� 2�� �������� ������ ��� : NOT�� ����
		if(cNode.getType() == NOT){
			ETree<Gtoken>* child2 = child->getFirstChild();
			curTreeNode->delDirectChildNode(child);
			parent->delDirectChildNode(curTreeNode);
			parent->addChild(child2);
			return true;
		}
		//1.2 NOT������ AND�� OR�� ������ AND�� OR�� ���� �ٲٰ� NOT�� �������� �ִ´�.
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

			curTreeNode->delDirectChildNode(child);  //And, OR ����
			parent->delDirectChildNode(curTreeNode); //not ����

			parent->addChild(orandNode);
			return true;
		}
		//1.3 NOT������ BOOL expr�� ���� NOT�� ���ְ� BOOL �����ڸ� �ݴ�� ��ȯ�Ѵ�.
		else if(cNode.getType() == BOOL){
			ETree<Gtoken>* child3 = child->getFirstChild()->getFirstChild(); //ù��°�� �����ڰ� �ִ�.
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

	//curTreeNode �� AND �Ǵ� OR�̸�
	else if(type == AND||type == OR){

		unsigned int cNum = curTreeNode->getChildrenNumber();

		//�ڽ��� �ϳ��̰� �� �ڽ��� AND�� OR�̸� ������ AND�� OR�� ���ش�.(DNF���� �߿� ������� �̻��� ���µ�)
		//A and (B or C) -> (A and B) or (A and C) �̴�.
		//���(A and B) or (A and C) ���´� (A and B) or (A and C) and nothing
		//A and (B or C) and D��츦 ������ ����...
		if(parent->getChildrenNumber() == 1 && cNum ==1){
			ETree<Gtoken>* child = curTreeNode->getFirstChild();
			parent->delDirectChildNode(curTreeNode);
			parent->addChild(child);
			return true;
		}

		//���� AND�� OR�� �ݺ��ؼ� ������ �ߺ��� ����� �ڽ��� �θ��� �ڽ����� ���δ�.
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


		//�ٷ� �Ʒ��� OR�� �ִ��� ���� �Ѵ�. (A or B) and C --> (A and C) or (B and C)
		if(cNum>1 && type == AND){
			bool hasOR =false;
			//child>1�̰� OR�� child�̸� �� child�� ù��°�� �ű��.
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
				ETree<Gtoken> *orp = curTreeNode->getFirstChild(); //�տ��� �Űܼ� ó���� or
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

			//�ٷ� �Ʒ��� NOT�� �ִ� ���
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

	//�׳� Ʈ�� �Ʒ���
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

