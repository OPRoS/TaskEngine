/*
 * Gcondition.cpp
 *
 *  Created on: 2009. 3. 2
 *      Author: jinseo
 */
#include "common/antlrIncl.h"
#include "stxmodel/Gcondition.h"

Gcondition::Gcondition(std::string file, int eol, std::string blockname, std::string blockpath):Gstmt(STMT_cond, file, eol, blockname, blockpath)
{
	m_cond = NULL;
}

Gcondition:: ~Gcondition()
{
	delete m_cond;
}

void Gcondition::setCondition(GstmtExpr *cond)
{
	if(m_cond != NULL)
		delete m_cond;

	m_cond = cond;
}

GstmtExpr* Gcondition::getCondition()
{
	return m_cond;
}

void Gcondition::toDNF(const std::string blockpath)
{
	bool changed = false;
	EXTree* eTree = m_cond->getExprTree();

	if(eTree->move2FirstChild()){
		do{
			EXTree* child = eTree->getCurrentChild();
			changed = iterDNF(eTree, child, blockpath);
			if(changed) break;
		}
		while(eTree->move2NextChild());
	}

	if(changed) toDNF(blockpath);
}

bool Gcondition::iterDNF(EXTree* parent, EXTree* curTreeNode, const std::string blockpath)
{
	//tree�� ����
	Gtoken token = curTreeNode->getNode();
	unsigned int type = token.getType();

	//1. NOT�� �������� ���� �־�� �Ѵ�. eg. !(!A)= A, !(!A and B) = (A or !B), !(A>B) = (A<=B)
	if(type == NOT){
		EXTree* child = curTreeNode->getFirstChild(); //NOT�� ���� ������
		Gtoken cNode = child->getNode();

		//1.1 NOT�� 2�� �������� ������ ��� : NOT�� ����
		if(cNode.getType() == NOT){
			EXTree* child2 = child->getFirstChild();
			curTreeNode->delDirectChildNode(child);
			parent->delDirectChildNode(curTreeNode);
			parent->addChild(child2);
			return true;
		}
		//1.2 NOT������ AND�� OR�� ������ AND�� OR�� ���� �ٲٰ� NOT�� �������� �ִ´�.
		else if(cNode.getType() == AND || cNode.getType() == OR){

			Gtoken orandToken;
			if(cNode.getType() == AND)orandToken = Gtoken(cNode.getFileName(), cNode.getBlockName(), cNode.getBlockPath(), "OR",-1, OR);
			if(cNode.getType() == OR)orandToken = Gtoken(cNode.getFileName(), cNode.getBlockName(), cNode.getBlockPath(), "AND",-1, AND);
			//ETree<Gtoken>* orandNode = new ETree<Gtoken>(orandToken);
			EXTree* orandNode = new EXTree(orandToken);

			if(child->move2FirstChild()){
				do{
					Gtoken notToken = Gtoken(cNode.getFileName(), cNode.getBlockName(), cNode.getBlockPath(), "NOT",-1, NOT);
					//ETree<Gtoken>* notNode = new ETree<Gtoken>(notToken);
					EXTree* notNode = new EXTree(notToken);
					notNode->addChild(child->getCurrentChild());
					orandNode->addChild(notNode);
				}
				while(child->move2NextChild());
			}

			curTreeNode->delDirectChildNode(child);  //And, OR ����
			parent->delDirectChildNode(curTreeNode); //not ����

			parent->addChild(orandNode);
			return true;
		}
		//1.3 NOT������ BOOL expr�� ���� NOT�� ���ְ� BOOL �����ڸ� �ݴ�� ��ȯ�Ѵ�.
		else if(cNode.getType() == BOOL){
			EXTree* child3 = child->getFirstChild()->getFirstChild(); //ù��°�� �����ڰ� �ִ�.
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
			EXTree* child = curTreeNode->getFirstChild();
			parent->delDirectChildNode(curTreeNode);
			parent->addChild(child);
			return true;
		}

		//���� AND�� OR�� �ݺ��ؼ� ������ �ߺ��� ����� �ڽ��� �θ��� �ڽ����� ���δ�.
		unsigned int pType =  parent->getNode().getType();
		if((pType==AND && type == AND) || (pType == OR && type ==OR)){
			if(curTreeNode->move2FirstChild()){
				do{
					EXTree* c = curTreeNode->getCurrentChild();
					parent->addChild(c);
				}
				while(curTreeNode->move2NextChild());
				parent->delDirectChildNode(curTreeNode);
				return true;
			}
		}


		//�ٷ� �Ʒ��� OR�� �ִ��� ���� �Ѵ�. (A or B) and C --> (A and C) or (B and C)
		if(cNum>1 && type == AND){
			bool hasOR =false;
			//child>1�̰� OR�� child�̸� �� child�� ù��°�� �ű��.
			//A and B and (C or D) --> (C or D) and A and B
			if(curTreeNode->move2FirstChild()){
				do{
					EXTree* child = curTreeNode->getCurrentChild();
					Gtoken childNode = child->getNode();
					if(childNode.getType()==OR){
						curTreeNode->cutCurrentChild();
						curTreeNode->addChildAsFirst(child);
						hasOR = true;
						break;
					}
				}
				while(curTreeNode->move2NextChild());
			}
			if(hasOR){
				// (C or D) and A and B  = > (C and A) or (D and A) and B
				EXTree* orp = curTreeNode->getFirstChild(); //�տ��� �Űܼ� ó���� or
				EXTree* second = curTreeNode->getNextChild();

				Gtoken newOrToken = Gtoken(orp->getNode().getFileName(), orp->getNode().getBlockName(), orp->getNode().getBlockPath(), orp->getNode().getData(),orp->getNode().getEndLine(),orp->getNode().getType());
				//ETree<Gtoken> *newOr = new ETree<Gtoken>(newOrToken);
				EXTree* newOr = new EXTree(newOrToken);

				if(orp->move2FirstChild()){
					do{
						Gtoken andp = Gtoken(orp->getNode().getFileName(), orp->getNode().getBlockName(), orp->getNode().getBlockPath(), "AND",-1, AND);
						//ETree<Gtoken>* andNode = new ETree<Gtoken>(andp);
						EXTree* andNode = new EXTree(andp);
						andNode->addChild(orp->getCurrentChild());
						andNode->addChild(second);
						newOr->addChild(andNode);
					}
					while(orp->move2NextChild());

				}
				//curTreeNode->clearCurChild();
				curTreeNode->cutChild(second);
				curTreeNode->delDirectChildNode(orp);
				curTreeNode->addChild(newOr);
				return true;
			}

			//�ٷ� �Ʒ��� NOT�� �ִ� ���
			if(curTreeNode->move2FirstChild()){
				do{
					EXTree* c = curTreeNode->getCurrentChild();
					Gtoken childNode = c->getNode();
					if(childNode.getType()==NOT){
						return iterDNF(curTreeNode, c, blockpath);
					}
				}
				while(curTreeNode->move2NextChild());
			}

		}

	}

	//�׳� Ʈ�� �Ʒ���
	if(curTreeNode->move2FirstChild()){
		do{
			EXTree* c = curTreeNode->getCurrentChild();
			iterDNF(curTreeNode, c, blockpath);
		}
		while(curTreeNode->move2NextChild());
	}
	else{
		//leafnode
	}


	return false;
}

void Gcondition::print(unsigned int indent)
{
	titlePrint(indent, std::string("COND"));

	if(m_cond != NULL){
		m_cond->print(indent+1);
	}
}
