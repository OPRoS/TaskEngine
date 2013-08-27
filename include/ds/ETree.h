#ifndef __ETREE_H__
#define __ETREE_H__

#include <string.h>
#include "EList.h"
#include "Edef.h"

#pragma warning (disable : 4996)

template<class T> class ETree
{

	private:
		static unsigned int NUMBER;
		unsigned int m_id; //expression에서 임의의 노드는 다른 부모에 의해서 참조될 수 있다. 만약 id가 같으면 같은 노드이다.
		unsigned int refcount;
		T m_node;
		EList<ETree<T>*> m_children;


		void buildTreeRefCount();
	
		unsigned int decreaseRef();
		unsigned int getRefCount();
		void countInit();
		void setCount(unsigned int);


	public:

		ETree(T node);
		virtual ~ETree(void);

		void setNode(T node);
		T getNode(void);
		bool isNodeEqual(T node);
		void reset();

		unsigned int increaseRef();
		void addChild(ETree<T> *child);
		void addChildAsFirst(ETree<T> *child);


		ETree<T>* getFirstChild(void);
		ETree<T>* getCurrentChild(void);
		ETree<T>* getNextChild(void);

		unsigned int getChildrenNumber();
		//EList<ETree<T>*> getChildren();

		bool moveFirstChild();
		bool moveNextChild();
		bool movePreviousChild();
		bool moveLastChild();

		unsigned int delCurChild();
		unsigned int cutCurChild();
		unsigned int cutChild(ETree<T> *child);
		unsigned int delDirectChildNode(ETree<T> *child);
		void delAll();

		void print(void);
		void print(int nbTabs);

};

template<class T>  unsigned int ETree<T>::NUMBER = 0;

template<class T> inline ETree<T>::ETree(T node)
{
	m_node = node;
	m_id = ++NUMBER;
	refcount = 0;
}

/* Base destructor */
static int kkk = 0;

template<class T> inline  ETree<T>:: ~ETree(void)
{
	std::vector<ETree<T>*> dellist;
	if (m_children.moveHead())
	{
kkk++;
printf("---------%d\n", kkk);
		do {
			ETree<T>* cont = m_children.getCurrent()->getContent();
			if(cont != NULL){
				cont->decreaseRef();
				if(cont->getRefCount() <= 1){				
					//delete cont;				
					//m_children.delCurrent();
					Gtoken tok = (Gtoken)(cont->getNode());
					
	printf("%s\n",tok.getData().c_str());
					dellist.push_back(cont);
				}				
			
			}
		}
		while (m_children.moveNext());
		//내용은 안 지움 내용지움은 m_children->clearAllchild()
	}

	std::vector<ETree<T>*>::iterator it;  
	for ( it=dellist.begin() ; it < dellist.end(); it++ ){
		
		delete *it;
	}

#ifdef __DEBUG_ETree_
	printf("ETree: virtual ~ETree();\n");
#endif
}

template<class T> inline void ETree<T>::delAll()
{

	//buildTreeRefCount();
	delete this;
}


template<class T> inline void ETree<T>::buildTreeRefCount()
{
	if (m_children.moveHead())
	{
		do {
			ETree<T>* cont = m_children.getCurrent()->getContent();
			if(cont != NULL){
				cont->increaseRef();//child를 참조하는 부모가 있기 때문에 child의 참조를 증가
				cont->buildTreeRefCount();
			}
		}
		while(m_children.moveNext());
	}
}


template<class T> inline void ETree<T>::setCount(unsigned int cnt)
{
	refcount = cnt;
}

/*
	트리내의 모든 reference count를 0으로 설정한다.
*/
template<class T> inline void ETree<T>::countInit()
{

	if (m_children.moveHead())
	{
		do {
			ETree<T>* cont = m_children.getCurrent()->getContent();
			cont->setCount(0);
			cont->countInit();
		}
		while(m_children.moveNext());
	}
}

template<class T> inline unsigned int ETree<T>::increaseRef()
{
	return ++refcount;
}

template<class T> inline unsigned int ETree<T>::decreaseRef()
{
	return --refcount;
}

template<class T> inline unsigned int ETree<T>::getRefCount()
{
	return refcount;
}

/* Set the node content */
template<class T> inline void ETree<T>::setNode(T node)
{
	m_node = node;
}
/* Get the node content */
template<class T> inline T ETree<T>::getNode(void)
{
	return m_node;
}

/*	Test the node content */
template<class T> inline bool ETree<T>::isNodeEqual(T node)
{
	return (node == m_node);
}

/* Add a child to the ETree */
template<class T> inline void ETree<T>::addChild(ETree<T> *child)
{
	child->increaseRef();
	m_children.addTail(child);
}

template<class T> inline void ETree<T>::addChildAsFirst(ETree<T> *child)
{
	m_children.addHead(child);
}

/* Return the first child */
template<class T> inline ETree<T> *ETree<T>::getFirstChild(void)
{
	if (m_children.moveHead())
		return m_children.getCurrent()->getContent();
	else
		return 0;
}

/* Return the next child */
template<class T> inline ETree<T> *ETree<T>::getNextChild(void)
{
	if (m_children.moveNext())
		return m_children.getCurrent()->getContent();
	else
		return 0;
}

template<class T> inline ETree<T> *ETree<T>::getCurrentChild(void)
{
	return m_children.getCurrent()->getContent();
}


template<class T> inline unsigned int  ETree<T>::getChildrenNumber(void)
{
	return m_children.getSize();
}

/*
	m_children에서 현재의 자식 노드 공간을 없애 버린다.
*/
template<class T> inline unsigned int  ETree<T>::cutCurChild()
{
	//return m_children.clearCurrentPtr();
	return m_children.delCurrent();
}

/**
	child와 연결된 포인터를 제거한다.
*/

template<class T> inline unsigned int ETree<T>::cutChild(ETree<T> *child){

	m_children.moveHead();
	do{
		if(m_children.getCurrent()->getContent()==child)
			//return m_children.clearCurrentPtr();
			return m_children.delCurrent();
	}
	while(m_children.moveNext());
	return 0;
}


/*
	현재 자식을 모두 NULL로 넣는다.
*/
template<class T> inline void  ETree<T>::reset()
{
	m_children.setAllNull();
}

/*
   바로아래 child만 삭제한다. child의 자식은 삭제하지 않는다.
   child를 가리키는 포인터를 m_children에서 clear하고 child를 삭제
*/
template<class T> inline unsigned int ETree<T>::delDirectChildNode(ETree<T> *child){

	if(m_children.getSize()>0){
		m_children.moveHead();
		do{
			if(m_children.getCurrent()->getContent()==child){
				//하나의 child는 ETree<Gtoken>*이다.
				child->reset();
				child->delAll();
				//delete child;
				//m_children.clearCurrentPtr();
				return m_children.delCurrent();
			}
		}
		while(m_children.moveNext());
	}
	return 0;
}

/*
	현재  child의 자식까지 모두 삭제한다.
*/

template<class T> inline unsigned int  ETree<T>::delCurChild(void)
{
	return m_children.delCurrent();
}


//template<class T> inline EList<ETree<T>*> ETree<T>::getChildren(void)
//{
//	return m_children;
//}

template<class T> inline bool ETree<T>::moveFirstChild(void)
{
	return m_children.moveHead();
}

template<class T> inline bool ETree<T>::moveNextChild(void)
{
	return m_children.moveNext();
}

template<class T> inline bool ETree<T>::movePreviousChild(void)
{
	return m_children.movePrevious();
}

template<class T> inline bool ETree<T>::moveLastChild(void)
{
	return m_children.moveTail();
}


template<class T> inline void ETree<T>::print(void)
{
	printf("------------------< TREE >----------------------\n");
	print(0);
	printf("-------------------------------------------------\n");
}

template<class T> inline void ETree<T>::print(int nbTabs)
{
	for (int i=0; i<nbTabs; i++) printf("|  ");

// m_node->getValue() 이부분은 ENode 클래스에 의존하는 것이다. 이부분을 동적으로 바인딩하게 해야하는데
// 아직 방법을 모르겠다.

	printf("|- nid%d", m_id);
	m_node.print();

	if (m_children.moveHead())
	{
		do m_children.getCurrent()->getContent()->print(nbTabs+1);
		while (m_children.moveNext());
	}
}


///* Destructor for T = (char *) : we must delete the string */
//template<class T> inline ETree<char *>:: ~ETree(void)
//{
//	if (m_children.moveHead())
//	{
//		do delete m_children.getCurrent()->getContent();
//		while (m_children.moveNext());
//	}
//	delete m_node;
//}
//
///*	Test the node content for T = (char *) */
//template<class T> inline bool ETree<char *>::isNodeEqual(char * node)
//{
//	return (strcasecmp(node, m_node) == 0);
//}



#endif

