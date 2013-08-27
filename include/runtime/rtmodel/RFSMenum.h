#pragma once

#include "runtime/rtmodel/RFSMsym.h"
#include <string>

class RFSMenum : public RFSMsym
{
private:
	int m_value;
	std::string m_typeStr;

public:
	RFSMenum(int, unsigned int, std::string, std::string, std::string file, int line);
	virtual ~RFSMenum(void);

	int getValue();
	unsigned int getType();
	std::string getTypeStr();

	std::string toString();
};
