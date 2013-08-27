#ifndef __LIST_H__
#define __LIST_H__

#include <stdio.h>
#include "EListElement.h"



template<class T> class EList
{
	private:
		unsigned int		m_size;
		EListElement<T>*	m_head;
		EListElement<T>*	m_curr;
		EListElement<T>*	m_tail;

		

	public:

		T operator [] (unsigned int index);

		EList(void);
		virtual ~EList(void);

		unsigned int addHead(T obj);
		unsigned int addTail(T obj);
		
		unsigned int delHead(void);
		unsigned int delCurrent(void);
		unsigned int delTail(void);

		void clearAllchild(void);
		unsigned int setAllNull(void);
		/*
		unsigned int clearHeadPtr();
		unsigned int clearCurrentPtr();
		unsigned int clearTailPtr();
		*/
		unsigned int getSize(void);
		unsigned int delElem(T obj);

		EListElement<T>* getHead(void);
		EListElement<T>* getCurrent(void);
		EListElement<T>* getTail(void);
		T getCurObject(void);

		bool moveHead(void);
		bool movePrevious(void);
		bool moveNext(void);
		bool moveTail(void);

		void print(void);
};



/* Base constructor */
template<class T> inline EList<T>::EList(void)
{
#ifdef __DEBUG_EList_
	printf("EList: EList();\n");
#endif
	m_size = 0;
	m_head = NULL;
	m_curr = NULL;
	m_tail = NULL;
}

/* Base destructor */
template<class T> inline EList<T>:: ~EList(void)
{
#ifdef __DEBUG_EList_
	printf("EList: virtual ~EList();\n");
#endif


	/************************************************************
		이부분이 문제가 자꾸 발생해서
		clear()에 해당하는 부분을 외부에서 직접 호출하도록 했다.
		(다수의 객체로 부터 참조되는 경우가 많아서.)
	/***********************************************************/
	//clear();	
}


///////////////////////////////////////////////////////////////////////

/* Add an object (inserted into an element) to the head of the EList */
template<class T> inline unsigned int EList<T>::addHead(T obj)
{
#ifdef __DEBUG_EList_
	printf("EList: BEGIN AddHead(%p)\n", obj);
#endif

	// A new element is created
	EListElement<T> *newElement = new EListElement<T>(obj);

	// The next element to the new one is the head
	newElement->setNext(m_head);
	// The head previous element is the new one
	if (m_head)	m_head->setPrevious(newElement);

	// The head element is now the new one
	m_head = newElement;

	// If this is the first element created in the EList
	if (m_size == 0)
	{
		// The current element is the new one
		m_curr = newElement;
		// The tail element is the new one
		m_tail = newElement;
	}

#ifdef __DEBUG_EList_
	printf("EList: END AddHead(%p)\n", obj);
#endif

	// There is one more element in the EList
	return ++m_size;
}

/* Add an object (inserted into an element) to the tail of the EList */
template<class T> inline unsigned int EList<T>::addTail(T obj)
{
#ifdef __DEBUG_EList_
	printf("EList: BEGIN AddTail(%p)\n", obj);
#endif

	// A new element is created
	EListElement<T> *newElement = new EListElement<T>(obj);

	// The previous element to the new one is the tail
	newElement->setPrevious(m_tail);
	// The tail next element is the new one
	if (m_tail)	
		m_tail->setNext(newElement);
	// The tail element is now the new one
	m_tail = newElement;


	// If this is the first element created in the EList
	if (m_size == 0)
	{
		// The current element is the new one
		m_curr = newElement;
		// The tail element is the new one
		m_head = newElement;
	}

#ifdef __DEBUG_EList_
	printf("EList: END AddTail(%p)\n", obj);
#endif

	// There is one more element in the EList
	return ++m_size;
}


///////////////////////////////////////////////////////////////////////

/* Delete the first element of the EList */
template<class T> inline unsigned int EList<T>::delHead(void)
{
#ifdef __DEBUG_EList_
	printf("EList: BEGIN delHead();\n");
#endif

	if (m_head != NULL)
	{
		// Create a temporary element pointing on the head next element
		EListElement<T> *nextElement = m_head->getNext();
		if (nextElement) nextElement->setPrevious(0);

		// The head element must be deleted
		delete (m_head);
m_head = NULL;

		// if the current element was pointing on the head it must now point on the temporary one
		if (m_curr == m_head) m_curr = nextElement;
		// if the tail element was pointing on on the head it must now point on the temporary one
		if (m_tail == m_head) m_tail = nextElement;
		// The head element is now the temporary one
		m_head = nextElement;

		// There is one less element in the EList
		return --m_size;
	}
	printf("EList: Error (delHead) : No head element in the EList\n");
	return 0;
}

/* Delete the current element of the EList */
template<class T> inline unsigned int EList<T>::delCurrent(void)
{
#ifdef __DEBUG_EList_
	printf("EList: delCurrent();\n");
#endif

	if (m_curr != NULL)
	{
		if (m_curr == m_head)
		{
#ifdef __DEBUG_EList_
			printf("EList: (current = head) -> ");
#endif
			return delHead();
		}
		
		if (m_curr == m_tail)
		{
#ifdef __DEBUG_EList_
			printf("EList: (current = tail) -> ");
#endif
			return delTail();
		}

		// Create a temporary element pointing on the current previous element
		EListElement<T> *prevElement = m_curr->getPrevious();
		// Create a temporary element pointing on the current next element
		EListElement<T> *nextElement = m_curr->getNext();

		// The current element must be deleted
		delete (m_curr);
m_curr= NULL;

		// The previous next element must point to the current next one
		prevElement->setNext(nextElement);
		// The next previous element must point to the current previous one
		nextElement->setPrevious(prevElement);
		// The current is now the previous element 
		m_curr = prevElement;

		// There is one less element in the EList
		return --m_size;
	}
	printf("EList: Error (delCurrent) : No current element in the EList\n");
	return 0;
}



/* Delete the last element of the EList */
template<class T> inline unsigned int EList<T>::delTail(void)
{
#ifdef __DEBUG_EList_
	printf("EList: delTail();\n");
#endif

	if (m_tail != NULL)
	{
		// Create a temporary element pointing on the tail previous element
		EListElement<T> *prevElement = m_tail->getPrevious();
		if (prevElement) prevElement->setNext(0);

		// The tail element must be deleted
		delete (m_tail);
m_tail = NULL;

		// if the current element was pointing on the tail it must now point on the temporary one
		if (m_curr == m_tail) m_curr = prevElement;
		// if the head element was pointing on on the tail it must now point on the temporary one
		if (m_head == m_tail) 
			m_head = prevElement;
		// The tail element is now the temporary one
		m_tail = prevElement;
	
		// There is one less element in the EList
		return --m_size;
	}
	printf("EList: Error (delTail) : No tail element in the EList\n");
	return 0;
}

template<class T> inline unsigned int EList<T>::delElem(T obj)
{
#ifdef __DEBUG_EList_
	printf("EList: del(%p)\n", obj);
#endif
	// Traverse the EList to delete the specified objects
	moveHead();
	do
	if (getCurrent()->getContent() == obj){ 
		delCurrent();
	}
	while (moveNext());

	return m_size;
}



///////////////////////////////////////////////////////////////////////

/* Get the first element of the EList */
template<class T> inline EListElement<T> *EList<T>::getHead(void)
{
	return m_head;
}

/* Get the current element of the EList */
template<class T> inline EListElement<T> *EList<T>::getCurrent(void)
{
	return m_curr;
}

template<class T> inline T EList<T>::getCurObject(void)
{
	return m_curr->getContent();
}

/* Get the last element of the EList */
template<class T> inline EListElement<T> *EList<T>::getTail(void)
{
	return m_tail;
}

/* Operator [] definition, access to an object in the EList by its index */
template<class T> inline T EList<T>::operator[] (unsigned int index)
{
#ifdef __DEBUG_EList_
	printf("EList: operator []\n");
#endif

	if (index < m_size)
	{
		EListElement<T> *tmpEl = m_head;
		for (unsigned int i=0; i<index; i++)
			tmpEl = tmpEl->getNext();
		return tmpEl->getContent();
	}
	
	printf("EList: Error (operator []) : Index out of bound\n");
	return 0;
}

///////////////////////////////////////////////////////////////////////

/* Set the current element pointer to the first of the EList */
template<class T> inline bool EList<T>::moveHead(void)
{
#ifdef __DEBUG_EList_
	printf("EList: moveHead();\n");
#endif

	return ((m_curr = m_head) != 0);
}

/* Set the current element pointer to the previous element in the EList */
template<class T> inline bool EList<T>::movePrevious(void)
{
#ifdef __DEBUG_EList_
	printf("EList: movePrevious();\n");
#endif

	if (m_curr)
		if (m_curr->getPrevious())
		{
			m_curr = m_curr->getPrevious();
			return true;
		}
	return false;
}

/* Set the current element pointer to the next element in the EList */
template<class T> inline bool EList<T>::moveNext(void)
{
#ifdef __DEBUG_EList_
	printf("EList: moveNext();\n");
#endif

	if (m_curr)
		if (m_curr->getNext())
		{
			m_curr = m_curr->getNext();
			return true;
		}
	return false;
}

/* Set the current element pointer to the last of the EList */
template<class T> inline bool EList<T>::moveTail(void)
{
#ifdef __DEBUG_EList_
	printf("EList: moveTail();\n");
#endif

	return ((m_curr = m_tail) != 0);
}



///////////////////////////////////////////////////////////////////////


/* Get the size of the EList */
template<class T> inline unsigned int EList<T>::getSize(void)
{
	return m_size;
}

/* Clear the EList */
template<class T> inline void EList<T>::clearAllchild(void)
{
#ifdef __DEBUG_EList_
	printf("EList: clear();\n");
#endif
	while(m_size > 0) delHead();
}


//////////////////////////////////////////////////////////////////////


/* Print the EList (as text) */
template<class T> inline void EList<T>::print(void)
{
#ifdef __DEBUG_EList_
	printf("EList: print();\n");
#endif

	EListElement<T> *tmpEl = m_head;
	
	printf("%d elements ", m_size);
	printf("{ ");
	while (tmpEl)
	{
		printf("%p", tmpEl->getContent());
		if (tmpEl == m_curr) printf("*");
		tmpEl = tmpEl->getNext();
		if (tmpEl) printf(", ");
	}
	printf(" }\n");
}




/*
template<class T> inline unsigned int EList<T>::clearHeadPtr()
{
#ifdef __DEBUG_EList_
	printf("EList: BEGIN cutHeadPtr();\n");
#endif

	if (m_head)
	{
		// Create a temporary element pointing on the head next element
		EListElement<T> *nextElement = m_head->getNext();
		if (nextElement) nextElement->setPrevious(0);

		// The head element must be deleted
		m_head=0;

		// if the current element was pointing on the head it must now point on the temporary one
		if (m_curr == m_head) m_curr = nextElement;
		// if the tail element was pointing on on the head it must now point on the temporary one
		if (m_tail == m_head) m_tail = nextElement;
		// The head element is now the temporary one
		m_head = nextElement;

		// There is one less element in the EList
		return --m_size;
	}
	printf("EList: Error (cutHeadPtr) : No head element in the EList\n");
	return 0;
}


template<class T> inline unsigned int EList<T>::clearTailPtr()
{
#ifdef __DEBUG_EList_
	printf("EList: cutTailPtr();\n");
#endif

	if (m_tail)
	{
		// Create a temporary element pointing on the tail previous element
		EListElement<T> *prevElement = m_tail->getPrevious();
		if (prevElement) prevElement->setNext(0);

		// The tail element must be deleted
		m_tail=0;

		// if the current element was pointing on the tail it must now point on the temporary one
		if (m_curr == m_tail) m_curr = prevElement;
		// if the head element was pointing on on the tail it must now point on the temporary one
		if (m_head == m_tail) m_head = prevElement;
		// The tail element is now the temporary one
		m_tail = prevElement;
	
		// There is one less element in the EList
		return --m_size;
	}
	printf("EList: Error (cutTailPtr) : No tail element in the EList\n");
	return 0;
}


template<class T> inline unsigned int EList<T>::clearCurrentPtr()
{
#ifdef __DEBUG_EList_
	printf("EList: cutCurPtr();\n");
#endif

	if (m_curr)
	{
		if (m_curr == m_head)
		{
#ifdef __DEBUG_EList_
			printf("EList: (current = head) -> ");
#endif
			return clearHeadPtr();
		}
		
		if (m_curr == m_tail)
		{
#ifdef __DEBUG_EList_
			printf("EList: (current = tail) -> ");
#endif
			return clearTailPtr();
		}

		// Create a temporary element pointing on the current previous element
		EListElement<T> *prevElement = m_curr->getPrevious();
		// Create a temporary element pointing on the current next element
		EListElement<T> *nextElement = m_curr->getNext();

		// The current element must be deleted
		m_curr=0;

		// The previous next element must point to the current next one
		prevElement->setNext(nextElement);
		// The next previous element must point to the current previous one
		nextElement->setPrevious(prevElement);
		// The current is now the previous element 
		m_curr = prevElement;

		// There is one less element in the EList
		return --m_size;
	}
	printf("EList: Error (delCurrent) : No current element in the EList\n");
	return 0;
}
*/

/*
	모든 list를 NULL로 셋팅한다.
*/
template<class T> inline unsigned int EList<T>::setAllNull(void)
{
#ifdef __DEBUG_EList_
	printf("EList: BEGIN delHead();\n");
#endif

	while(m_size > 0){
		if (m_head != NULL)
		{
			// Create a temporary element pointing on the head next element
			EListElement<T> *nextElement = m_head->getNext();
			if (nextElement) nextElement->setPrevious(0);

			// if the current element was pointing on the head it must now point on the temporary one
			if (m_curr == m_head) m_curr = nextElement;
			// if the tail element was pointing on on the head it must now point on the temporary one
			if (m_tail == m_head) m_tail = nextElement;

			// The head element must be deleted
			m_head = 0;

			// The head element is now the temporary one
			m_head = nextElement;

			// There is one less element in the EList
			--m_size;
		}
	}
	m_tail = 0;
	m_curr = 0;
//	printf("EList: Error (delHead) : No head element in the EList\n");
	return m_size;
	
}


#endif


