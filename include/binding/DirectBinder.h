#pragma once

#include <map>
#include <string>
#include "common/TypeDef.h"
#include "binding/ModelBinder.h"
#include "runtime/rtmodel/RFSMcall.h"
#include "run/TaskConfig.h"

#ifdef _WIN32
#include <windows.h>
typedef HINSTANCE LIB_HANDLER;
#define dlopen(path,flag)     LoadLibrary((path));
#define dlsym(handle,module)  GetProcAddress((handle),(module));
#define dlclose(handle)       FreeLibrary((HINSTANCE)(handle));
#endif

#ifndef _WIN32
#include <dlfcn.h>
typedef void* LIB_HANDLER;
#define LoadLibrary(path)     dlopen((path), RTLD_LAZY);
#define GetProcAddress(handle,module)  dlsym((handle),(module));
#define FreeLibrary(handle)   dlclose((handle));
#endif

class DirectBinder : public ModelBinder
{
private:
	std::map<std::string, LIB_HANDLER> m_LibTAB;
	//LIB_HANDLER hInstDll;

	LIB_HANDLER getLibHandler(std::string);
	StrMap		getParameters(ValueMap*);

public:
	DirectBinder(void);
	virtual ~DirectBinder(void);
	
	RFSMvalueEval			callFunction(LogFile*, int, std::string, int, unsigned int, std::string, ValueMap*);
	virtual void			setModelValue(LogFile*, std::string, RFSMvalueEval);
	virtual RFSMvalueEval	getModelValue(LogFile*, RFSMvariable*, int line);

	static const unsigned int CT_IN		= 100;
	static const unsigned int CT_OUT	= 200;

};
