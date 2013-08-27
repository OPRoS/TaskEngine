#pragma once

#include "common/TypeDef.h"
#include "run/TaskConfig.h"
#include "log/LogFile.h"
#include "log/Log.h"
#include "runtime/rtmodel/RFSMvalueEval.h"
#include "runtime/rtmodel/RFSMvariable.h"


class ModelBinder
{
private:
	ValueMap	m_gVarTAB;//gvar �𵨰��� �����

protected:
	//pthread_mutex_t* m_mutexp
	Log			m_log;	

public:
	ModelBinder(void);
	virtual ~ModelBinder(void);
	
	//��������
	virtual void setGlobalValue(LogFile*, std::string, RFSMvalueEval);
	virtual RFSMvalueEval getGlobalValue(LogFile*, std::string name);

	//�ɺ�
	virtual void setModelValue(LogFile*, std::string, RFSMvalueEval)=0;
	virtual RFSMvalueEval getModelValue(LogFile*, RFSMvariable*, int)=0;
	
	//�Լ�
	virtual RFSMvalueEval callFunction(LogFile*, int callType, std::string target, int line, unsigned int rtnType, std::string fName, ValueMap* params)=0;

	//virtual void setModelValueImpl(LogFile*, std::string, RFSMvalueEval)=0;
	//virtual RFSMvalueEval getModelValueImpl(LogFile*, RFSMvariable*)=0;
	//virtual RFSMvalueEval callFunctionImpl(LogFile*, int callType, unsigned int rtnType, std::string fName, ValueMap* params)=0;

};
