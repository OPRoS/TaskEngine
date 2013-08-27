#pragma once

#include <map>
#include <string>
#include <vector>
#include "common/TypeDef.h"
#include "util/Utility.h"

class MonitorRequest
{
private:	
	unsigned int	m_type;		//req, stop, end, resume, status
	StrMap			m_map;		//ATTR에 포함된 @속성이름, 값
	//std::string		m_taskName;
	std::string		m_key;		//task.bhv.state.entry에서 task가 빠진것이 key가 된다.

public:
	MonitorRequest(int cmdType);
	virtual ~MonitorRequest(void);

	unsigned int setAttributes(std::string attrMsg);
	std::string getAttrValue(std::string key);
	std::string removeBracket(std::string attr);
	//std::string getTaskName();
	std::string getKey();
	unsigned int getType();
//	unsigned int getAfter();
//	int getIteration();	
};

static unsigned int CMD_register 	= 1;
static unsigned int CMD_remove		= 2;
static unsigned int CMD_start 		= 3;
static unsigned int CMD_suspend		= 4;
static unsigned int CMD_resume 		= 5;
static unsigned int CMD_stop 		= 6;

//emulation
static unsigned int CMD_set			= 7;
static unsigned int CMD_emul		= 8;

static std::string ATTR_target		= "target";
static std::string ATTR_file		= "file";
static std::string ATTR_task		= "task";
static std::string ATTR_line		= "line";
static std::string ATTR_thread		= "thread";
static std::string ATTR_stepbystep	= "stepbystep";
static std::string ATTR_isMonitoring= "isMonitoring";

static std::string ATTR_msgid		= "msgid";
static std::string ATTR_time		= "time";
static std::string ATTR_name		= "name";
static std::string ATTR_type		= "type";
static std::string ATTR_value		= "value";

static std::string ATTR_targetOn	= "on";
static std::string ATTR_targetOff	= "off";

static std::string ATTR_thid		= "thid";
static std::string ATTR_parent		= "parent";
static std::string ATTR_cmd			= "cmd";


static std::string ATTR_min		= "min";
static std::string ATTR_max		= "max";
static std::string ATTR_enumlist= "enumlist";
static std::string ATTR_etype	= "etype";

static std::string BLOCK_entry	= "entry";
static std::string BLOCK_trans	= "trans";
static std::string BLOCK_stay	= "stay";
static std::string BLOCK_exit	= "exit";

static std::string E_enum		= "enum";
static std::string E_value		= "value";
static std::string E_randomF_U	= "ramdomF_U";
static std::string E_randomF_G	= "randomF_G";
static std::string E_randomD_U	= "randomD_U";
static std::string E_randomD_G	= "randomD_G";


static unsigned int REQ_ATTR_ERROR_no_target	= 100;