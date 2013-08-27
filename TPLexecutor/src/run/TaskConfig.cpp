/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2008.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/

#include "run/TaskConfig.h"

int		TaskConfig::m_mode;
int		TaskConfig::m_binderMode;
bool	TaskConfig::m_telsvr;	
bool	TaskConfig::m_modelCodeGen;	
//bool	TaskConfig::m_monsvr;
int		TaskConfig::m_numCli;
int		TaskConfig::m_logsize;
bool	TaskConfig::m_isConfiged;
bool	TaskConfig::m_isTaskLogging;
bool	TaskConfig::m_isThreadTracing;
bool	TaskConfig::m_isAutoDeploy;
bool	TaskConfig::m_autoResultPrint;
bool	TaskConfig::m_usingQueueTimer;
bool	TaskConfig::m_loggingTimePeriod;

std::string		TaskConfig::m_executorHome;
std::string		TaskConfig::m_deployHome;

std::string		TaskConfig::m_deployFile; //자동실행할 때 사용
std::string		TaskConfig::m_autoRunTask;

unsigned short	TaskConfig::m_cliport = 0;
//unsigned short	TaskConfig::m_ftpport = 0;
//unsigned short  TaskConfig::m_monitorport = 0;

TaskConfig::TaskConfig()
{
}

TaskConfig:: ~TaskConfig()
{	
}

void TaskConfig::init()
{
	std::string fname = "tpl.cfg";
	m_isConfiged = false;
	m_autoRunTask = "";

	m_cfgfile.open(fname.c_str());
	if(m_cfgfile.is_open()){

		std::string	line;
		while (!m_cfgfile.eof()){
			getline (m_cfgfile, line);

			if(line =="" || line[0]=='#') continue;
			std::vector<std::string> path = Estring::split(line, "=");
			std::string lstr;
			std::string rstr;
			
			if(path.size()>0){
			  lstr = path[0];
			  lstr = Estring::trim(lstr);
			}
			
			if(path.size()>1){
			  rstr = path[1];
			  rstr = Estring::trim(rstr);
			}

			if(lstr == "deployMode"){
			  if(rstr == "overwrite"){
				  m_mode = TaskConfig::OVERWRITE;
			  }
			  else if(rstr == "unique"){
				  m_mode = TaskConfig::UNIQUE;
			  }
			  else if(rstr == "addition"){
				  m_mode = TaskConfig::ADDITION;
			  }
			  else{//default 
				  m_mode = TaskConfig::OVERWRITE;
			  }			  
			}

			if(lstr == "logSize"){
				m_logsize = str2int(rstr);
			}

			if(lstr == "cliport"){
				m_cliport = (unsigned short)(str2int(rstr));
			}

			//if(lstr == "ftpport"){
			//	m_ftpport = (unsigned short)str2int(rstr);
			//}

			//if(lstr == "monitorport"){
			//	m_monitorport = (unsigned short)str2int(rstr);
			//}
			
			if(lstr == "telnetSvr"){
			  if(rstr=="on") m_telsvr = true;
			  else m_telsvr = false;
			}

			if(lstr == "modelCodeGen"){
			  if(rstr=="yes") m_modelCodeGen = true;
			  else m_modelCodeGen = false;
			}

			if(lstr == "usingQueueTimer"){
			  if(rstr=="yes") m_usingQueueTimer = true;
			  else m_usingQueueTimer = false;
			}

			//if(lstr == "monitorSvr"){
			//  if(rstr=="on") m_monsvr = true;
			//  else m_monsvr = false;
			//}

			if(lstr == "autoDeploy"){
			  if(rstr=="on") m_isAutoDeploy = true;
			  else m_isAutoDeploy = false;
			}

			if(lstr == "autoResultPrint"){
			  if(rstr=="on") m_autoResultPrint = true;
			  else m_autoResultPrint = false;
			}

			if(lstr == "numOfTelClient"){
				m_numCli = str2int(rstr);
			}
			
			if(lstr == "binder"){
			  if(rstr=="direct"){
				  m_binderMode = TaskConfig::DIRECT;
			  }
			  else if(rstr=="plugin"){
				  m_binderMode = TaskConfig::PLUG_IN;
			  }
			  else if(rstr=="component"){
				  m_binderMode = TaskConfig::COMPONENT;
			  }
			  else{//default
				  m_binderMode = TaskConfig::PLUG_IN;
			  }
			}

			if(lstr == "executorHome"){
				m_executorHome = rstr;
				m_deployHome = rstr; // 초기에 deployHome을 executorHome으로 설정
			}

			if(lstr == "deployHome"){
				m_deployHome = rstr;
			}

			if(lstr == "deployFile"){
				m_deployFile = rstr;
			}

			if(lstr == "autoRunTask"){
				m_autoRunTask = rstr;
			}

			if(lstr == "loggingTask"){
				if(rstr=="on") m_isTaskLogging = true;
				else m_isTaskLogging = false;
			}

			if(lstr == "loggingTimePeriod"){
				if(rstr=="on") m_loggingTimePeriod = true;
				else m_loggingTimePeriod = false;
			}
			
			if(lstr == "traceThread"){
				if(rstr=="on") m_isThreadTracing = true;
				else m_isThreadTracing = false;
			}
			
		}
	 
		m_cfgfile.close();
		m_isConfiged = true;
	}
	else{
		//tpl.cfg파일이 존재하지 않는 경우
	}
	
/*	
	if(m_home==""){
		 m_home = getCurPath();
	}
	else
	{
		std::string tempHome;

		tempHome = getCurPath();
		tempHome.append("/");
		tempHome.append(m_home);

		m_home = tempHome;
	}
*/
}

bool TaskConfig::isAutoDeploy()
{
	return m_isAutoDeploy;
}

bool TaskConfig::isAutoResultPrint()
{
	return m_autoResultPrint;
}

bool TaskConfig::isUsingQueueTimer()
{
	return m_usingQueueTimer;
}

bool TaskConfig::isloggingTimePeriod()
{
	return m_loggingTimePeriod;
}

int TaskConfig::getDeployMode()
{	
	return m_mode;
}

void TaskConfig::setDeployMode(int mode)
{
	m_mode = mode;
}

int TaskConfig::getBindingMode()
{	
	return m_binderMode;
}

std::string TaskConfig::getDeployFile()
{
	return m_deployFile;
}

std::string TaskConfig::getAutoRunTask()
{
	return m_autoRunTask;
}

bool TaskConfig::getIsTaskLog()
{
	return m_isTaskLogging;
}

bool TaskConfig::getIsThreadTrace()
{
	return m_isThreadTracing;
}

void TaskConfig::setBindingMode(int mode)
{
	m_binderMode = mode;
}

bool TaskConfig::isTelServerOn()
{
	return m_telsvr;
}

bool TaskConfig::getModelCodeGen()
{
	return m_modelCodeGen;
}

//bool TaskConfig::isMonitorSvrOn()
//{
//	return m_monsvr;
//}

int TaskConfig::getLogSize()
{
	return m_logsize;
}

std::string TaskConfig::getExecutorHome()
{
	if(m_executorHome == "")
		return getCurPath();

	return m_executorHome;
}

std::string TaskConfig::getDeployHome()
{
	return m_deployHome;
}

void TaskConfig::setDeployHome(std::string deployHome)
{
	m_deployHome = deployHome;
}

int TaskConfig::getNumOfCLISession()
{
	return m_numCli;
}

bool TaskConfig::beSetted()
{
	return m_isConfiged;
}

unsigned short TaskConfig::getCLIport()
{
	return m_cliport;
}

//unsigned short TaskConfig::getFTPport()
//{
//	return m_ftpport;
//}

//unsigned short TaskConfig::getMonitorport()
//{
//	return m_monitorport;
//}