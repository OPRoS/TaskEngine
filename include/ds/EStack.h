/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include <iostream>

template <class T> class EStack
{
	T *stack;
	int	top;
	int max_stack_size;

public:
	EStack(int sz);
	virtual ~EStack(void);

	void push(T i);
	T pop();
	int empty();
	int is_full();
	int depth();
	void print();
};


template <class T> EStack<T>::EStack(int sz)
{
	stack = new T[sz];
	top = 0;
	max_stack_size = sz;
}

template <class T> EStack<T>:: ~EStack(void)
{
	delete[] stack;
}

template <class T> void EStack<T>::push(T i)
{
	if(top<max_stack_size)
		(stack)[top++]=i;
	else
		std::cerr<<"ouverflow pushing"<<i<<std::endl;
}

template <class T> T EStack<T>::pop()
{
	if(top>0)
		return (stack)[--top];

	std::cerr<<"stack underflow\n";
	return 0;
}

template <class T> int EStack<T>::empty()
{
	return top==0;
}

template <class T> int EStack<T>::is_full()
{
	return top==max_stack_size;
}

template <class T> int EStack<T>::depth()
{
	return top;
}

template <class T> void EStack<T>::print()
{
	for(unsigned int i=0;i<top;i++)
		std::cout<<stack[i]<<" ";
	std::cout<<std::endl;
}

/*
void EParser::testStack()
{
	
	EStack<char> strStack = EStack<char>(8);
	strStack.push('a');
	std::cout<<strStack.pop()<<std::endl;

	Gtoken *a = new Gtoken(m_curfName, std::string("a"), 1, 1);
	Gtoken *b = new Gtoken(m_curfName, std::string("b"), 2, 2);
	test(a, b);
	a->print();
	b->print();
}
*/