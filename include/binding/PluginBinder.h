#pragma once

#ifdef USE_PLUGIN

#include "rpf/rpf.h"
#include "binding/ModelBinder.h"
#include "thread/RThread.h"

using namespace rpf;

class PluginBinder : public enable_rpf_object<PluginBinder>, public RThread, public ModelBinder
{
public:
	PluginBinder(void);
	virtual ~PluginBinder(void);

	void setModelValue(LogFile* lf, std::string, RFSMvalueEval);
	RFSMvalueEval getModelValue(LogFile* lf, RFSMvariable*, int);	
	RFSMvalueEval callFunction(LogFile* lf, int callType, std::string, int, unsigned int rtnType, std::string funcName, ValueMap*);

	//PluggableBase
	int init(); 
	
	//RThread
	void cbackBy_Thrfunc();

};

#endif