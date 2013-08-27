/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2011.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2011 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#ifdef USE_OXIL

#pragma comment(lib, "oxil.lib")

#include "binding/RemoteCompBinder.h"

RemoteCompBinder::RemoteCompBinder(std::string ip, std::string port)
{
	m_oxil.SetUp(ip, port);
}

RemoteCompBinder::~RemoteCompBinder(void)
{
}


RFSMvalueEval RemoteCompBinder::callFunction(LogFile* log, int callType, std::string blockname, int line, unsigned int rtnType, std::string name, ValueMap* paramList)
{
//	pthread_mutex_lock(m_mutexp);

	RFSMvalueEval vEval(line, blockname);

	//1. 파라메터 변환
	std::string void_str("v");
	std::stringstream outstring;
	opros::archive::oarchive *param = m_oxil.getEncoder(&outstring, 1);
	
	if(paramList != NULL){
		int i;
		int sz = paramList->size();
		std::map<std::string, RFSMvalueEval>::iterator it;

		for (i=0;i<sz;i++){									
			for(it=paramList->begin() ; it!=paramList->end(); it++){
				std::string pname = it->first;
				RFSMvalueEval eval = it->second;
				if (eval.getIndex()==i){
					if (eval.getType() == TYP_BOOL) {
						bool val= eval.getBoolValue();
						(*param) << val;
					}
					else if(eval.getType() == TYP_INT || eval.getType()==TYP_SHORT || eval.getType() == TYP_LONG){
						int val= eval.getIntValue();
						(*param) << val;
					}
					else if(eval.getType() == TYP_FLOAT || eval.getType() == TYP_DOUBLE){
						double val = eval.getFloatValue();
						(*param) << val;
					}
					else if(eval.getType() == TYP_STRING){
						std::string val = eval.getStrValue();
						(*param) << val;
					}
					break;
				}
			}
		}	
	}
	else{
		(*param) << void_str;
	}

	//2. 함수 호출
	int retEncode = 0;
	std::string retV;
	if (m_oxil.symbolCall(name, outstring.str(), retEncode, retV) != OPROS_NO_ERROR){
		return vEval;
	}

	//3. 리턴값
	//RetType rtData;
	std::stringstream r_v(retV);
	opros::archive::iarchive *pia = m_oxil.getDecoder(&r_v, retEncode);

	if(rtnType == TYP_INT){
		int result;
		(*pia) >> result;
		vEval.setIntValue(result);
	}
	if(rtnType == TYP_BOOL){	
		bool result;
		(*pia) >> result;
		vEval.setBoolValue(result);
	}
	if(rtnType == TYP_FLOAT || rtnType == TYP_DOUBLE){
		float result;
		(*pia) >> result;
		vEval.setFloatValue(result);
	}
	if(rtnType == TYP_STRING){
		std::string result;
		(*pia) >> result;
		vEval.setStrValue(result);
	}
	if(rtnType ==TYP_VOID){
		

	}

	
	if(callType==CALL_FUNC)
		FLOG_TRACE(log, "   (PLUG-IN:FUNCTION) Calling function:%s, return:%s", name.c_str(), vEval.getStrValue().c_str());
	if(callType==CALL_ACTION)
		FLOG_TRACE(log, "   (PLUG-IN:ACTION) Calling action:%s, return:%s", name.c_str(), vEval.getStrValue().c_str());
	
//	pthread_mutex_unlock(m_mutexp);

	return vEval;
}

void RemoteCompBinder::setModelValue(LogFile* log, std::string name, RFSMvalueEval value)
{


	if(value.getType() == TYP_INT){
		
	

		//pIntFunc(value.getIntValue());
	}
	if(value.getType() == TYP_FLOAT){

		

		//pFloatFunc(value.getFloatValue());
	}
	if(value.getType() ==TYP_STRING){

		
		//pStrFunc(value.getStrValue());
	}

	FLOG_TRACE(log, "   (PLUG-IN:OUT) Calling function:%s, return:%s", name.c_str(), value.getStrValue().c_str());
	
}

RFSMvalueEval RemoteCompBinder::getModelValue(LogFile* log, RFSMvariable* var, int line)
{
	RFSMvalueEval eval(line, var->getBlockName());

	unsigned int type = var->getType();
	std::string name = var->getName();

	if(type == TYP_INT){
		eval = callFunction(log, CT_IN, var->getBlockName(), line, TYP_INT, name, NULL);				
		FLOG_TRACE(log, "   (PLUG-IN:MODEL, IN) Type:int, name:%s, value:%d", name.c_str(), eval.getIntValue());
		
	}
	else if(type == TYP_FLOAT){
		eval = callFunction(log, CT_IN, var->getBlockName(), line, TYP_FLOAT, name, NULL);		
		FLOG_TRACE(log, "   (PLUG-IN:MODEL, IN) Type:float, name:%s, value:%f", name.c_str(), eval.getFloatValue());
		

	}
	else if(type == TYP_STRING){
		eval = callFunction(log, CT_IN, var->getBlockName(), line, TYP_STRING, name, NULL);	
		FLOG_TRACE(log, "   (PLUG-IN:MODEL, IN) Type:string, name:%s, value:%s", name.c_str(), eval.getStrValue().c_str());
	
	}

	return eval;
}

#endif