#pragma once

#ifdef USE_OXIL

#include "binding/ModelBinder.h"
#include "thread/RThread.h"
#include "runtime/rtmodel/RFSMcall.h"
#include <oxil.h>

class RemoteCompBinder : public ModelBinder
{
private:
	OPRoSLegacyInterface  m_oxil;

public:
	RemoteCompBinder(std::string ip, std::string port);
	virtual ~RemoteCompBinder(void);

	//void setGlobalValue(LogFile*, std::string, RFSMvalueEval);
	//RFSMvalueEval getGlobalValue(LogFile*, std::string name);

	//심볼
	void setModelValue(LogFile*, std::string, RFSMvalueEval);
	RFSMvalueEval getModelValue(LogFile*, RFSMvariable*, int);
	
	//함수
	RFSMvalueEval callFunction(LogFile*, int callType, std::string target, int line, unsigned int rtnType, std::string fName, ValueMap* params);

	static const unsigned int CT_IN		= 100;
	static const unsigned int CT_OUT	= 200;
};

#endif