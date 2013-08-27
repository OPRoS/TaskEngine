/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#pragma once

#include <string.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <cassert>
#include <locale>

typedef std::basic_string<wchar_t> tstring;

class Estring
{
private:
	char* m_str;

public:

	Estring(char*);
	virtual ~Estring(void);

	void setValue(char*);
	char* getValue();

	static  std::string toString(int);

	void	strcpy(Estring*);
	int		strlen();
	int		strcmp(Estring*);
	void	strcat(Estring*);
	static void replaceAll(std::string& text, const std::string& find_token, const std::string& replace_token);
	static void replaceAll(std::string* text, const std::string& find_token, const std::string& replace_token);
	static std::string trim(const std::string str);	
	static std::vector<std::string> split(const std::string & s, const std::string & delim);
	static std::vector<std::string> split(const std::string & s, char delim);
	static std::string wcs2str(std::wstring const&, std::locale const&);
	static std::wstring wcs2wstr(std::string const&, std::locale const&);
	
};
