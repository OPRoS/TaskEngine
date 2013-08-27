/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "binding/ModelBinder.h"

ModelBinder::ModelBinder(void)
{
//-	m_mutexp = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
//-	pthread_mutex_init(m_mutexp, NULL);
	m_log.setLevel( Log::LOG_LEVEL_DEBUG, true);
	m_log.setLevel( Log::LOG_LEVEL_TRACE, true);
	m_log.setLevel( Log::LOG_LEVEL_ERROR, true);
}

ModelBinder:: ~ModelBinder(void)
{
	//-free(m_mutexp);
}


void ModelBinder::setGlobalValue(LogFile* lf, std::string varName, RFSMvalueEval valueEval)
{
	//-pthread_mutex_lock(m_mutexp);

	FLOG_TRACE(lf, "  (RUNNING:GVar, Write) %s, %s", varName.c_str(), valueEval.getStrValue().c_str());

	ValueMap::iterator it = m_gVarTAB.find(varName);
	if(it != m_gVarTAB.end()){
		RFSMvalueEval eval = it->second;
		m_gVarTAB.erase(it);
	}
	m_gVarTAB.insert(ValuePair(varName, valueEval));

	//-pthread_mutex_unlock(m_mutexp);
}

RFSMvalueEval ModelBinder::getGlobalValue(LogFile* lf, std::string name)
{
	ValueMap::iterator it = m_gVarTAB.find(name);
	if(it != m_gVarTAB.end()){
		RFSMvalueEval eval = it->second;
		FLOG_TRACE(lf, "  (RUNNING:GVar, Read) %s, %s", name.c_str(), eval.getStrValue().c_str());
		return eval;
	}
	else{
		FLOG_ERROR(lf, "  (RUNNING:GVar, Read) %s, Not initialized", name.c_str());
		return RFSMvalueEval();
	}
}