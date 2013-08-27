#pragma once

#include "runtime/rtmodel/RFSMboolable.h"
#include <string>
#include <vector>

class RFSMlogicOp
{
private:
	std::vector<RFSMboolable*>*	m_List;

public:
	RFSMlogicOp(void);
	virtual ~RFSMlogicOp(void);

	void addBoolExpr(RFSMboolable*);
	std::vector<RFSMboolable*>* getBoolExprs();

	static std::string toString(RFSMlogicOp*);
};


