/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "binding/DirectBinder.h"

DirectBinder::DirectBinder(void)
{	
	//hInstDll = getLibHandler("Model"); 
}

/*
여러개의 task에서 하나의 dll을 동시 참조 할 수 있기 때문에
CLI를 완전히 빠져나갈 때 소멸자가 호출된다.
*/
DirectBinder:: ~DirectBinder(void)
{	
	if(m_LibTAB.size()>0){
		std::map<std::string, LIB_HANDLER>::iterator it;
		for(it=m_LibTAB.begin() ; it!=m_LibTAB.end() ; it++){
			#ifdef _WIN32
				FreeLibrary(it->second); // Pass.dll 해제							
			#endif
			#ifndef _WIN32
				dlclose(it->second);
			#endif
		}
		m_LibTAB.clear();	
	}
}

LIB_HANDLER DirectBinder::getLibHandler(std::string modelName)
{

	LIB_HANDLER libHandler = NULL;

	std::map<std::string, LIB_HANDLER>::iterator it = m_LibTAB.find(modelName);

	if(it != m_LibTAB.end()){
		return it->second;
	}
	else{
		// DLL의 인스턴스 핸들, home directory의 /direct아래에 dll파일이 존재한다.
		//std::string dlldir = TaskConfig::getHomeDir();
		std::string dlldir = TaskConfig::getExecutorHome();
		dlldir.append("/direct/dll/").append(modelName.c_str()); 
		
		#ifdef _WIN32
		//hInstDll = LoadLibraryA("E:\\package\\OPRoS_TASK\\Project_Milestone\\Debug\\Model_.dll");	// Pass.dll 로드	
		dlldir.append(".dll");
		libHandler = LoadLibraryA(dlldir.c_str());
		//char errormsg[100];
		//FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, errormsg, sizeof(errormsg), NULL);
		//printf("%s\n", errormsg);
		#endif
		#ifndef _WIN32
		dlldir.append(".so");
		libHandler = dlopen(dlldir.c_str(), RTLD_LAZY);
		#endif
				
		if(libHandler != NULL)
			m_LibTAB.insert(std::pair<std::string, LIB_HANDLER>(modelName, libHandler));
		else{
		#ifdef _WIN32
			char errormsg[100];
			FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 0, errormsg, sizeof(errormsg), NULL);
			printf("%s\n", errormsg);
		#endif
		}
	}
	
	return libHandler;
}

StrMap DirectBinder::getParameters(ValueMap* paramList)
{
	StrMap pmap;
	if(paramList != NULL){
		std::map<std::string, RFSMvalueEval>::iterator it;
		for(it=paramList->begin() ; it!=paramList->end() ; it++){
			std::string pname = it->first;
			RFSMvalueEval eval = it->second;
			std::string value = eval.getStrValue();
			pmap.insert(StrPair(pname, value));
		}
	}

	return pmap;
}

RFSMvalueEval DirectBinder::callFunction(LogFile* log, int callType, std::string blockname, int line, unsigned int rtnType, std::string name, ValueMap* paramList)
{
//	pthread_mutex_lock(m_mutexp);

	RFSMvalueEval vEval(line, blockname);

	typedef std::string (*strFunc)	(StrMap);
	typedef int			(*intFunc)	(StrMap);
	typedef float		(*floatFunc)(StrMap);
	typedef void		(*voidFunc)	(StrMap);
	typedef bool		(*boolFunc)	(StrMap);

	//이름 추출
	std::vector<std::string> pathexpr = Estring::split(name, ".");
	

	//LIB_HANDLER hInstDll = getLibHandler("Model"); 
	std::string modelname = "";
	for(unsigned int i=0; i<pathexpr.size()-1; i++){
		if(i!=0) modelname.append("_");
		modelname = modelname.append(pathexpr[i]);
	}

	LIB_HANDLER libHandle = getLibHandler(modelname); 
	if(libHandle == NULL)  {		
		//에러 메시지 출력
		LOG_ERROR(m_log, "TaskExecutor cannot find the library for Model<%s>", modelname.c_str());			
		return vEval;
	}	

	std::string funName = modelname;
	if(callType==CT_IN){
		//eg. obstacle_getleft
		funName.append("_in_").append(pathexpr[pathexpr.size()-1]);
	}
	else if(callType==CT_OUT){
		//eg. obstacle_setleft
		funName.append("_out_").append(pathexpr[pathexpr.size()-1]);
	}
	else if(callType==CALL_FUNC || callType == CALL_ACTION){
		//eg. obstacle_distance
		funName.append("_").append(pathexpr[pathexpr.size()-1]);
	}

	StrMap ps = getParameters(paramList);
	if(rtnType == TYP_INT){
		
		int (*pIntFunc)(StrMap);
		#ifdef _WIN32
		pIntFunc = (intFunc) GetProcAddress(libHandle, funName.c_str());// 형변환 해야함
		#endif
		#ifndef _WIN32
		pIntFunc = (intFunc)dlsym(libHandle, funName.c_str());
		#endif
		if(pIntFunc == NULL){
			LOG_ERROR(m_log, "<%s.%s> is not defined", pathexpr[0].c_str(), pathexpr[1].c_str());
			return vEval;
		}

		int result = pIntFunc(ps);
		vEval.setIntValue(result);
	}
	if(rtnType == TYP_BOOL){
		
		int (*pBoolFunc)(StrMap);
		#ifdef _WIN32
		pBoolFunc = (intFunc) GetProcAddress(libHandle, funName.c_str());// 형변환 해야함
		#endif
		#ifndef _WIN32
		pIntFunc = (boolFunc)dlsym(libHandle, funName.c_str());
		#endif
		if(pBoolFunc == NULL){
			LOG_ERROR(m_log, "<%s.%s> is not defined", pathexpr[0].c_str(), pathexpr[1].c_str());
			return vEval;
		}

		bool result = pBoolFunc(ps);
		vEval.setBoolValue(result);
	}
	if(rtnType == TYP_FLOAT || rtnType == TYP_DOUBLE){

		float (*pFloatFunc)(StrMap);
		#ifdef _WIN32
		pFloatFunc = (floatFunc) GetProcAddress(libHandle, funName.c_str());// 형변환 해야함
		#endif
		#ifndef _WIN32
		pFloatFunc = (floatFunc)dlsym(libHandle, funName.c_str());
		#endif
		if(pFloatFunc == NULL){
			LOG_ERROR(m_log, "<%s.%s> is not defined", pathexpr[0].c_str(), pathexpr[1].c_str());
			return vEval;
		}

		float result = pFloatFunc(ps);
		vEval.setFloatValue(result);
	}
	if(rtnType == TYP_STRING){

		std::string (*pStrFunc)(StrMap);
		#ifdef _WIN32
		pStrFunc = (strFunc) GetProcAddress(libHandle, funName.c_str());// 형변환 해야함
		#endif
		#ifndef _WIN32
		pStrFunc = (strFunc)dlsym(libHandle, funName.c_str());
		#endif
		if(pStrFunc == NULL){
			LOG_ERROR(m_log, "<%s.%s> is not defined", pathexpr[0].c_str(), pathexpr[1].c_str());
			return vEval;
		}

		std::string result = pStrFunc(ps);
		vEval.setStrValue(result);
	}
	if(rtnType ==TYP_VOID){
		
		void (*pVoidFunc)(StrMap);
		#ifdef _WIN32
		pVoidFunc = (voidFunc) GetProcAddress(libHandle, funName.c_str());// 형변환 해야함
		#endif
		#ifndef _WIN32
		pVoidFunc = (voidFunc)dlsym(libHandle, funName.c_str());
		#endif
		if(pVoidFunc == NULL){
			LOG_ERROR(m_log, "<%s.%s> is not defined", pathexpr[0].c_str(), pathexpr[1].c_str());
			return vEval;
		}

		pVoidFunc(ps);
	}

	
	if(callType==CALL_FUNC)
		FLOG_TRACE(log, "   (PLUG-IN:FUNCTION) Calling function:%s, return:%s", name.c_str(), vEval.getStrValue().c_str());
	if(callType==CALL_ACTION)
		FLOG_TRACE(log, "   (PLUG-IN:ACTION) Calling action:%s, return:%s", name.c_str(), vEval.getStrValue().c_str());
	
//	pthread_mutex_unlock(m_mutexp);

	return vEval;
}

void DirectBinder::setModelValue(LogFile* log, std::string name, RFSMvalueEval value)
{
//	pthread_mutex_lock(m_mutexp);

	typedef void (*strFunc)		(std::string);
	typedef void (*intFunc)		(int);
	typedef void (*floatFunc)	(float);

	//이름 추출
	std::vector<std::string> pathexpr = Estring::split(name, ".");
	std::string funName = pathexpr[0];

	LIB_HANDLER libHandle = getLibHandler(pathexpr[0]); 
	if(libHandle == NULL)  {		
		//에러 메시지 출력
		LOG_ERROR(m_log, "TaskExecutor cannot find the library for Model<%s>", pathexpr[0].c_str());			
		return;
	}	

	funName.append("_out_").append(pathexpr[1]);

	if(value.getType() == TYP_INT){
		
		void (*pIntFunc)(int);
		#ifdef _WIN32
		pIntFunc = (intFunc) GetProcAddress(libHandle, funName.c_str());// 형변환 해야함
		#endif
		#ifndef _WIN32
		pIntFunc = (intFunc)dlsym(libHandle, funName.c_str());
		#endif
		if(pIntFunc == NULL){
			LOG_ERROR(m_log, "<%s.%s> is not defined", pathexpr[0].c_str(), pathexpr[1].c_str());
			return;
		}

		pIntFunc(value.getIntValue());
	}
	if(value.getType() == TYP_FLOAT){

		void (*pFloatFunc)(float);
		#ifdef _WIN32
		pFloatFunc = (floatFunc) GetProcAddress(libHandle, funName.c_str());// 형변환 해야함
		#endif
		#ifndef _WIN32
		pFloatFunc = (floatFunc)dlsym(libHandle, funName.c_str());
		#endif
		if(pFloatFunc == NULL){
			LOG_ERROR(m_log, "<%s.%s> is not defined", pathexpr[0].c_str(), pathexpr[1].c_str());
			return;
		}

		pFloatFunc(value.getFloatValue());
	}
	if(value.getType() ==TYP_STRING){

		void (*pStrFunc)(std::string);
		#ifdef _WIN32
		pStrFunc = (strFunc) GetProcAddress(libHandle, funName.c_str());// 형변환 해야함
		#endif
		#ifndef _WIN32
		pStrFunc = (strFunc)dlsym(libHandle, funName.c_str());
		#endif
		if(pStrFunc == NULL){
			LOG_ERROR(m_log, "<%s.%s> is not defined", pathexpr[0].c_str(), pathexpr[1].c_str());
			return;
		}

		pStrFunc(value.getStrValue());
	}

	FLOG_TRACE(log, "   (PLUG-IN:OUT) Calling function:%s, return:%s", name.c_str(), value.getStrValue().c_str());
	
//	pthread_mutex_unlock(m_mutexp);
}

RFSMvalueEval DirectBinder::getModelValue(LogFile* log, RFSMvariable* var, int line)
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