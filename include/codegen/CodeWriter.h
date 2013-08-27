#ifndef CODEWRITER_H_
#define CODEWRITER_H_

#include <map>
#include "ds/Estring.h"
#include "filenavi/OSFileSys.h"

class CodeWriter
{
protected:
	OSFileSys*		m_fsys;
	FILE*			m_file_style;
	std::string		m_homeDir;
	std::map<std::string, std::string>				m_ptable;
	std::map<std::string, std::string>::iterator	m_it;	
	typedef std::pair <std::string, std::string>	strpair;
	
public:
	CodeWriter(OSFileSys*);
	virtual ~CodeWriter();

	void setHomeDir(std::string);
	int initStyle(std::string);
	std::string getBlock(std::string);
};

#endif /*CODEWRITER_H_*/
