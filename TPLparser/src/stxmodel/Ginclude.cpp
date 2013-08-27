/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "stxmodel/Ginclude.h"

Ginclude::Ginclude(void): EList<Gtoken*>()
{		
}

Ginclude:: ~Ginclude(void)
{		
	while(moveHead()){
		Gtoken* tok = getCurObject();
		delete tok;
		delHead();
	}		

	while(m_checkedFiles.moveHead()){
		m_checkedFiles.delHead();
	}		
}

void Ginclude::add(std::string fileName, std::string inclName)
{
	if(!exist(inclName)){
		Gtoken* tok = new Gtoken(fileName,"", "", inclName, -1,-1);
		addTail(tok);
	}
}

bool Ginclude::exist(std::string name)
{
	if(moveHead()){
		do{
			Gtoken* tok = getCurObject();
			std::string str = tok->getData();
			if(name.compare(str)==0)
				return true;
		}
		while(moveNext());
	}
	return false;
}

void Ginclude::checked(std::string fname)
{
	if(!isChecked(fname))
		m_checkedFiles.addTail(fname);
}

bool Ginclude::isChecked(std::string fname)
{
	if(m_checkedFiles.moveHead()){
		do{
			std::string checkedfile = m_checkedFiles.getCurObject();
			if(checkedfile.compare(fname)==0)
				return true;
		}
		while(m_checkedFiles.moveNext());
	}
	return false;
}

EList<std::string> Ginclude::getIncludeFiles()
{
	return m_checkedFiles;
}

/*
void Ginclude::addFilename(Gtoken* fToken)
{
	if(m_fnamelist == NULL)
		m_fnamelist = new EList<Gtoken*>();

	m_fnamelist->addTail(fToken);
}

EList<Gtoken*>* Ginclude::getAllfilenames()
{
	return m_fnamelist;
}
*/
