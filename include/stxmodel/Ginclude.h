/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include "Gtoken.h"

class Ginclude : public EList<Gtoken*>
{
private:
	EList<std::string> m_checkedFiles;

public:
	Ginclude(void);
	virtual ~Ginclude(void);

	//void addFilename(Gtoken*);
	//EList<Gtoken*>* getAllfilenames();

	void add(std::string, std::string);
	void checked(std::string filename);
	bool exist(std::string);
	bool isChecked(std::string);
	EList<std::string> getIncludeFiles(void);
};
