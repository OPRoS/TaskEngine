#include "comm/MonitorResponse.h"


MonitorResponse::MonitorResponse()
{	
	m_num = 1;
	pthread_mutex_init(&m_mu, NULL);
}

MonitorResponse::~MonitorResponse()
{
	pthread_mutex_destroy(&m_mu);
}

void MonitorResponse::init(unsigned int type)
{
	m_type = type;
	m_data = "";
}

void MonitorResponse::setDataType(unsigned int type)
{
	m_type = type;
}

std::string MonitorResponse::makeResponseMsg(std::string taskname, int threadID, int threadline, std::string file, int dbgLine, std::string target, int line)
{
	pthread_mutex_lock(&m_mu);
	std::ostringstream ostr;

	ostr<<"RES-start;";
	
	//0. type	
	if(m_type==RES_DATA_in)			ostr<<"in;";
	else if(m_type==RES_DATA_out)	ostr<<"out;";
	else if(m_type==RES_DATA_wvar)	ostr<<"wvar;";
	else if(m_type==RES_DATA_gvar)	ostr<<"gvar;";
	else if(m_type==RES_DATA_lvar)	ostr<<"lvar;";
	else if(m_type==RES_DATA_func)	ostr<<"func;";
	else if(m_type==RES_DATA_param) ostr<<"param;";
	else if(m_type==RES_DATA_trans) ostr<<"trans;";
	else if(m_type==RES_DATA_empty) ostr<<"empty;";

	//1. 일련번호
	ostr<<"@msgid["<<m_num<<"]";

	ostr<<"@task["<<taskname<<"]";

	ostr<<"@thread["<<threadID<<"]";

	ostr<<"@threadLine"<<"["<<int2str(threadline)<<"]";

	//2. date & time
	time_t timer= time(NULL);
	struct tm *t = localtime(&timer);
	int hour = t->tm_hour;
	int min	 = t->tm_min;
	int sec	 = t->tm_sec;
	ostr<<"@time["<<hour<<"-"<<min<<"-"<<sec<<"]";

	//4. file
	ostr<<"@file["<<file<<"]";

	ostr<<"@dbgline["<<dbgLine<<"]";

	if(line != -1){//empty 디버거 메시지
		//3. target
		ostr<<"@target["<<target<<"]";
		
		//5. line
		ostr<<"@line["<<line<<"]";
	

		//6. DATA에 대한 속성/
		ostr<<m_data;
	}

	ostr<<";RES-end";

	ostr<<"\r\n";

	m_num++;
	pthread_mutex_unlock(&m_mu);
	return ostr.str();
}

std::string MonitorResponse::makeThreadMsg(std::string thid, std::string name, std::string pthid)
{
	pthread_mutex_lock(&m_mu);
	std::ostringstream ostr;

	ostr<<"RES-start;thread:";

	//1. 일련번호
	ostr<<"@thread["<<thid<<"]";

	//2. date & time
	time_t timer= time(NULL);
	struct tm *t = localtime(&timer);
	int hour = t->tm_hour;
	int min	 = t->tm_min;
	int sec	 = t->tm_sec;
	ostr<<"@time["<<hour<<"-"<<min<<"-"<<sec<<"]";

	//3. name
	ostr<<"@name["<<name<<"]";

	//4. parent
	ostr<<"@parent["<<pthid<<"]";

	ostr<<"RES-end";

ostr<<"\r\n";

pthread_mutex_unlock(&m_mu);
	return ostr.str();
}

void MonitorResponse::addFunc(std::string fname)
{
	m_data = "@name[" + fname + "]";
}

void MonitorResponse::addFuncParam(std::string fname, RFSMvalueEval pVal)
{
	m_data = m_data + makeVarAttribute(fname, pVal);
}

void MonitorResponse::addFuncReturn(std::string fname, RFSMvalueEval rVal)
{
}

void MonitorResponse::addVariable(std::string varName, RFSMvalueEval val)
{
	m_data = m_data + makeVarAttribute(varName, val);
}

std::string MonitorResponse::makeVarAttribute(std::string varname, RFSMvalueEval val)
{
	int type = val.getType();
	std::string typeStr;
	if(type == TYP_INT) typeStr = "int";
	else if(type == TYP_FLOAT) typeStr = "float";
	else if(type == TYP_STRING) typeStr = "string";
	else if(type == TYP_BOOL) typeStr = "bool";	

	std::string msg = "@name[" + varname + "]" + "@type[" + typeStr + "]" + "@value[" + val.getStrValue() + "]";

	return msg;
}