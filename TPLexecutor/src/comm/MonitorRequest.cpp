#include "comm/MonitorRequest.h"
#include "common/TypeDef.h"
#include "ds/Estring.h"

MonitorRequest::MonitorRequest(int cmdType)
{	
	m_type = cmdType;
}

MonitorRequest::~MonitorRequest(void)
{
}

unsigned int MonitorRequest::getType()
{
	return m_type;
}

unsigned int MonitorRequest::setAttributes(std::string attrMsg)
{
	std::vector<std::string> attrs = Estring::split(attrMsg, "@");	
	std::vector<std::string>::iterator it = attrs.begin();

	std::string target	= "";
	std::string file	= "";
	std::string task	= "";
	std::string line	= "";
	std::string thread	= "";
	std::string stepbystep		= "";
	std::string isMonitoring	= "";

	std::string msgid	= "";
	std::string time	= "";
	std::string name	= "";
	std::string type	= "";
	std::string value	= "";
	std::string targetOn	= "";
	std::string targetOff	= "";

	std::string thid	= "";
	std::string parent	= "";
	std::string cmd		= "";

	std::string etype		= "";
	std::string min			= "";
	std::string max			= "";
	std::string enumlist	= "";

	for(it=attrs.begin()+1 ; it!=attrs.end() ; it++){//it는 ATTR:라는 문자열이 이다. 그 다음부터
		std::string attr = (*it);	

		if(attr.find(ATTR_target)==0){//target
			target = removeBracket(attr);
			m_map.insert(StrPair(ATTR_target, target));
			
			/*
			//task이름 저장
			std::vector<std::string> path = Estring::split(target, ".");
			if(path.size()>0)
				m_taskName = path[0];
			//key저장
			size_t as = target.find_first_of(".");
			m_key = target.substr(as+1, target.size());
			m_map.insert(StrPair(ATTR_target, target));
			*/
		}
		else if(attr.find(ATTR_file)==0){//file
			file = removeBracket(attr);
			m_map.insert(StrPair(ATTR_file, file));
		}
		else if(attr.find(ATTR_task)==0){ //task
			task = removeBracket(attr);		
			m_map.insert(StrPair(ATTR_task, task));
		}
		else if(attr.find(ATTR_line)==0){ //line
			line = removeBracket(attr);	
			m_map.insert(StrPair(ATTR_line, line));
		}
		else if(attr.find(ATTR_thread)==0){ //thread
			thread = removeBracket(attr);			
			m_map.insert(StrPair(ATTR_thread, thread));
		}
		else if(attr.find(ATTR_stepbystep)==0){ //stepbystep
			stepbystep = removeBracket(attr);			
			m_map.insert(StrPair(ATTR_stepbystep, stepbystep));
		}
		else if(attr.find(ATTR_isMonitoring)==0){ //isMonitoring
			isMonitoring = removeBracket(attr);			
			m_map.insert(StrPair(ATTR_isMonitoring, isMonitoring));
		}
		else if(attr.find(ATTR_msgid)==0){ //msgid
			msgid = removeBracket(attr);			
			m_map.insert(StrPair(ATTR_msgid, msgid));
		}
		else if(attr.find(ATTR_value)==0){ //value
			value = removeBracket(attr);			
			m_map.insert(StrPair(ATTR_value, value));
		}
		else if(attr.find(ATTR_targetOn)==0){
			targetOn = removeBracket(attr);			
			m_map.insert(StrPair(ATTR_targetOn, targetOn));
		}
		else if(attr.find(ATTR_targetOff)==0){
			targetOff = removeBracket(attr);			
			m_map.insert(StrPair(ATTR_targetOff, targetOff));
		}
		if(attr.find(ATTR_etype)==0){
			etype = removeBracket(attr);			
			m_map.insert(StrPair(ATTR_etype, etype));
		}
		if(attr.find(ATTR_min)==0){
			min = removeBracket(attr);			
			m_map.insert(StrPair(ATTR_min, min));
		}
		if(attr.find(ATTR_max)==0){
			max = removeBracket(attr);			
			m_map.insert(StrPair(ATTR_max, max));
		}
		if(attr.find(ATTR_enumlist)==0){
			enumlist = removeBracket(attr);			
			m_map.insert(StrPair(ATTR_enumlist, enumlist));
		}
	}	

	return 0;
}

/*
std::string MonitorRequest::getTaskName()
{
	return m_taskName;
}
*/
std::string MonitorRequest::getKey()
{
	return m_key;
}

std::string MonitorRequest::removeBracket(std::string attr)
{
	size_t i1 = attr.find("[");
	size_t i2 = attr.find("]");
	if(i1>0 && i2>0){
		std::string val = attr.substr(i1+1, i2-i1-1);
		return val;
	}
	return "";
}

std::string MonitorRequest::getAttrValue(std::string key)
{
	std::map<std::string, std::string>::iterator it = m_map.find(key);
	if(it != m_map.end()){
		std::string value = it->second;
		return value;
	}

	return "";
}

/*
unsigned int MonitorRequest::getAfter()
{
	std::map<std::string, std::string>::iterator it = m_map.find(REQ_ATTR_after);
	if(it != m_map.end()){
		std::string value = it->second;
		return (unsigned int)(str2int(value));		
	}

	return 0;
}

int MonitorRequest::getIteration()
{
	std::map<std::string, std::string>::iterator it = m_map.find(REQ_ATTR_iteration);
	if(it != m_map.end()){
		std::string value = it->second;
		if(value == REQ_ATTR_CONST_Unlimit)
			return -1;
		else
			return str2int(value);
	}

	return -1;
}
*/