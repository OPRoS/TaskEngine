#ifndef __ELISTELEMENT_H__
#define __ELISTELEMENT_H__

#include <stdio.h>
template<class T> class IsPointer {
public:
	enum { value = false };
};

template<class T> class IsPointer<T*> {
public:
	enum { value = true };
};


template<class T> class EListElement
{
	private:
		T	m_obj; //ETree<Gtoken>
		EListElement<T>* m_prev;
		EListElement<T>* m_next;

		enum { isPointer = IsPointer<T>::value };

	public:
		EListElement(T obj);
		virtual ~EListElement(void);


		void setContent(T obj);
		void setPrevious(EListElement<T> *prev);
		void setNext(EListElement<T> *next);

		T getContent(void);
		EListElement<T>* getPrevious(void);
		EListElement<T>* getNext(void);
};


/* Base constructor */
template<class T> inline EListElement<T>::EListElement(T obj)
{
	m_obj = obj;
	m_prev = NULL;
	m_next = NULL;
}

/* Base destructor */
template<class T> inline EListElement<T>:: ~EListElement(void)
{
	//if(isPointer && m_obj != NULL){
	//	delete m_obj;
	//}
	//T 자동으로 소멸자 호출
}


/* Get the element content */
template<class T> inline T EListElement<T>::getContent(void)
{
	return (m_obj);
}

/* Set the element content */
template<class T> inline void EListElement<T>::setContent(T obj)
{
	m_obj = obj;
}


/* Get the previous element */
template<class T> inline EListElement<T>* EListElement<T>::getPrevious(void)
{
	return (m_prev);
}

/* Set the previous element */
template<class T> inline void EListElement<T>::setPrevious(EListElement<T>* prev)
{
	m_prev = prev;
}


/* Get the next element */
template<class T> inline EListElement<T>* EListElement<T>::getNext(void)
{
	return (m_next);
}

/* Set the next element */
template<class T> inline void EListElement<T>::setNext(EListElement<T>* next)
{
	m_next = next;
}


#endif