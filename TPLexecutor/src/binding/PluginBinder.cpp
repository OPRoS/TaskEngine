/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2011.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2011 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#ifdef USE_PLUGIN

#ifdef _DEBUG
#pragma comment(lib, "rpf_local_d.lib")
#else
#pragma comment(lib, "rpf_local.lib")
#endif

#include "comm/RemoteCLISvr.h"//winsock2.h window.h 충돌 회피
#include "binding/PluginBinder.h"
#include "run/TaskConfig.h"

PluginBinder::PluginBinder():RThread("PluginBinder", 10)
{
	if(TaskConfig::getIsTaskLog()){	
		m_log.setLevel(Log::LOG_LEVEL_DEBUG, true);
		m_log.setLevel(Log::LOG_LEVEL_TRACE, true);
		m_log.setLevel(Log::LOG_LEVEL_ERROR, true);
	}
}

PluginBinder:: ~PluginBinder(void)
{
}


RFSMvalueEval PluginBinder::getModelValue(LogFile* lf, RFSMvariable* rvar, int line)
{
	RFSMvalueEval eval(line, rvar->getBlockName());

	unsigned int type = rvar->getType();
	std::string name = rvar->getName();

	rpf_var v;

	if(type == TYP_INT){

		try {
			int intval = rpf_get_val(name);
			eval.setIntValue(intval);
			FLOG_TRACE(lf, "   (PLUG-IN:MODEL, IN) Type:int, name:%s, value:%d", name.c_str(), eval.getIntValue());
		} catch (...)
		{
			FLOG_ERROR(lf, "   (PLUG-IN:MODEL, IN) Type:int, name:%s, value:%d", name.c_str());
		}
	}
	if(type == TYP_FLOAT){
		try{
			float f_val = rpf_get_val(name);
			eval.setFloatValue(f_val);
			FLOG_TRACE(lf, "   (PLUG-IN:MODEL, IN) Type:float, name:%s, value:%f", name.c_str(), eval.getFloatValue());
		} catch (...)
		{
			FLOG_ERROR(lf, "   (PLUG-IN:MODEL, IN) Type:float, name:%s, value:%f", name.c_str());
		}

	}
	if(type == TYP_STRING){
		try{
			std::string strval = (std::string)rpf_get_val(name);
			eval.setStrValue(strval);
			FLOG_TRACE(lf, "   (PLUG-IN:MODEL, IN) Type:string, name:%s, value:%s", name.c_str(), eval.getStrValue().c_str());
		} catch (...)
		{
			FLOG_ERROR(lf, "   (PLUG-IN:MODEL, IN) Type:string, name:%s, value:%s", name.c_str());
		}
	}

	return eval;

}

void PluginBinder::setModelValue(LogFile* lf, std::string name, RFSMvalueEval eval)
{
	unsigned int type = eval.getType();

	FLOG_TRACE(lf, "(PLUG-IN:MODEL, OUT) Setting model value");

	if(type == TYP_INT){
		try{
			rpf_set_val(name, eval.getIntValue());
			FLOG_TRACE(lf, "   (PLUG-IN:MODEL, OUT) Type:int, name:%s , value:%s", name.c_str(), eval.getStrValue().c_str());
		} catch (...)
		{
			FLOG_ERROR(lf, "   (PLUG-IN:MODEL, OUT) Type:int, name:%s", name.c_str());
		}
		// LOG LOG LOG
	}
	if(type == TYP_FLOAT){
		try{
			rpf_set_val(name, eval.getFloatValue());
			FLOG_TRACE(lf, "   (PLUG-IN:MODEL, OUT) Type:float, name:%s, value:%s", name.c_str(), eval.getStrValue().c_str());
		} catch (...)
		{
			FLOG_ERROR(lf, "   (PLUG-IN:MODEL, OUT) Type:float, name:%s", name.c_str());
		}
		// LOG LOG
	}
	if(type == TYP_STRING){
		try{
			rpf_set_val(name, eval.getStrValue());
			FLOG_TRACE(lf, "   (PLUG-IN:MODEL, OUT) Type:string, name:%s, value:%s", name.c_str(), eval.getStrValue().c_str());
		} catch (...)
		{
			FLOG_ERROR(lf, "   (PLUG-IN:MODEL, OUT) Type:string, name:%s", name.c_str());
		}
	}
}


//   AnyList params;
//	params.push_back(AnyValue(1));
//	params.push_back(AnyValue(1.2));
//	POLog(logger::debug, "[Sample] Calling PsychicPiranea.getVal");
//	int i = OPCallFunction(&val"PsychicPiranea.getVal", params);


RFSMvalueEval PluginBinder::callFunction(LogFile* lf, int callType, std::string blockname, int line, unsigned int rtnType, std::string funcName, ValueMap* paramList)
{
//-pthread_mutex_lock(m_mutexp);
	RFSMvalueEval rtnEval(line, blockname);

	rpf_var val;


	if(paramList != NULL){
		rpf_list anyParams;
		std::map<std::string, RFSMvalueEval>::iterator it;
		for(it=paramList->begin() ; it!=paramList->end() ; it++){
			std::string pname = it->first;
			RFSMvalueEval eval = it->second;

			//입력 파라메터 변환
			if(eval.getType() == TYP_INT){
				int val= eval.getIntValue();
				anyParams.push_back(val);
			}
			if(eval.getType() == TYP_FLOAT){
				float val = eval.getFloatValue();
				anyParams.push_back(val);
			}
			if(eval.getType() == TYP_STRING){
				std::string val = eval.getStrValue();
				anyParams.push_back(val);
			}
		}


	    //함수호출
		if(rtnType == TYP_INT){
			try {
				int val = rpf_funcall(funcName, anyParams);
				rtnEval.setIntValue(val);
			} catch (...)
			{
				FLOG_ERROR(lf, "   (PLUG-IN:FUNC/ACTION) returnType:int name:%s value:%s", funcName.c_str());
			}
		}
		if(rtnType == TYP_FLOAT){
			try {
				rtnEval.setFloatValue(rpf_funcall(funcName, anyParams));
			} catch (...)
			{
				FLOG_ERROR(lf, "   (PLUG-IN:FUNC/ACTION) returnType:float name:%s", funcName.c_str());
			}

		}
		if(rtnType == TYP_STRING){
			try {
				rtnEval.setStrValue(rpf_funcall(funcName, anyParams));
			} catch (...)
			{
				FLOG_ERROR(lf, "   (PLUG-IN:FUNC/ACTION) returnType:string name:%s", funcName.c_str());
			}
		}
		if(rtnType == TYP_VOID){
			try {
				rpf_funcall(funcName, anyParams);
			} catch (...)
			{
				FLOG_ERROR(lf, "   (PLUG-IN:FUNC/ACTION) returnType:void name:%s", funcName.c_str());
			}
		}

		FLOG_TRACE(lf, "   (PLUG-IN:FUNC/ACTION) Calling (function/Action):%s, return:%s", funcName.c_str(), rtnEval.getStrValue().c_str());

//-pthread_mutex_unlock(m_mutexp);		
		return rtnEval;
	}
	else{

		if(rtnType == TYP_INT){
			try {
				int val = rpf_funcall(funcName);
				rtnEval.setIntValue(val);

			} catch (...)
			{
				FLOG_ERROR(lf, "   (PLUG-IN:FUNC/ACTION) returnType:int name:%s", funcName.c_str());
			}
		}
		if(rtnType == TYP_FLOAT){
			try {
				rtnEval.setFloatValue(rpf_funcall(funcName)) ;
			} catch (...)
			{
				FLOG_ERROR(lf, "   (PLUG-IN:FUNC/ACTION) returnType:float name:%s", funcName.c_str());
			}
		}
		if(rtnType == TYP_STRING){
			try {
				rtnEval.setStrValue(rpf_funcall(funcName));
			} catch (...)
			{
				FLOG_ERROR(lf, "   (PLUG-IN:FUNC/ACTION) returnType:string name:%s", funcName.c_str());
			}
		}
		if(rtnType == TYP_VOID){
			try {
				rpf_funcall(funcName);
			} catch (...)
			{
				FLOG_ERROR(lf, "   (PLUG-IN:FUNC/ACTION) returnType:void name:%s");
			}
		}
		
		if(callType==CALL_FUNC)
		FLOG_TRACE(lf, "   (PLUG-IN:FUNCTION) Calling function:%s, return:%s", funcName.c_str(), rtnEval.getStrValue().c_str());

		if(callType==CALL_ACTION)
		FLOG_TRACE(lf, "   (PLUG-IN:ACTION) Calling action:%s, return:%s", funcName.c_str(), rtnEval.getStrValue().c_str());

//-pthread_mutex_unlock(m_mutexp);

		return rtnEval;
	}

//-pthread_mutex_unlock(m_mutexp);
	return rtnEval;
}

int PluginBinder::init()
{
	TaskConfig cfg;
	cfg.init();
	
//	PO_CONNECT_FUNCTION(TaskManagerPI, getWorkerList);
//	OP_PUBLISH_FUNCTION(TaskManagerPI, loadWorker);
///	OP_PUBLISH_FUNCTION(TaskManagerPI, unloadWorker);
//	OP_PUBLISH_FUNCTION(TaskManagerPI, startWorker);
//	OP_PUBLISH_FUNCTION(TaskManagerPI, stopWorker);
//	OPUpdate();

	if(TaskConfig::beSetted())
		this->create_thr(RThread::ATTACH,"PLUGIN_BINDER");
	else
		printf("Because of configuration problems, PluginBinder cannot start Task Executor.\n");

	return 0;
}

void PluginBinder::cbackBy_Thrfunc()
{
	m_running = true;

	/* 홈디렉토리 설정*/	
	OSFileSys fsys;
	fsys.open();
	//std::string homeDir = TaskConfig::getHomeDir();
	//if(homeDir == "") homeDir = getCurPath();
	//fsys.setDir(homeDir);
	fsys.setDir(TaskConfig::getDeployHome());

	if(TaskConfig::isTelServerOn()){
		printf("[PluginBinder] OPRoS TaskExecutor is now started.\n");
		//plug-in에서는 tpl.cfg에 상관없이 telnet과 binder가 자동으로 지정된다.
		std::auto_ptr<RemoteCLISvr> rc(new RemoteCLISvr(this, &fsys));
		rc->create_thr(RThread::ATTACH, "REMOTE_CLI_SVR");

//		std::auto_ptr<Rftp> rftp(new Rftp());
//		rftp->create_thr();

		//join
		rc->wait4join();
//		rftp->cleanUp();
	}
	else{
		std::auto_ptr<LocalCLI> lc(new LocalCLI(NULL, NULL, &fsys, this));
		lc->run();
	}


	m_running = false;
}

#endif