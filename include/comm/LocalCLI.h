#pragma once

#include <map>
#include "runtime/rtmodel/RFSMvariable.h"
#include "runtime/rtmodel/RFSMcall.h"
#include "run/TaskManager.h"
#include "run/control/TaskRunner.h"
#include "run/TaskConfig.h"
#include "binding/DirectBinder.h"
#include "binding/PluginBinder.h"
#include "comm/MonitorRequest.h"
#include "common/TypeDef.h"
#include "filenavi/OSFileSys.h"
#include "log/LogFile.h"

//class TaskManager;

class LocalCLI
{
private:
	TaskManager*	m_taskMgr;
	ModelBinder*	m_binder;
	//ValueMap		m_gVarTAB;		//gvar¸ðµ¨°ª ÀúÀå
	std::ostream*	m_ostr;
	OSFileSys*		m_fsys;

	//std::string m_curWorker;
	Log				m_log;

	//bool m_isInited;
	//bool m_isStarted;

public:
	LocalCLI(RemoteCLI*, std::ostream*, OSFileSys*, ModelBinder*);
	virtual ~LocalCLI(void);
	
	void autoLoad		(void);
	int  loadWorker		(std::string);
	void unloadWorker	(std::string);
	int	 startWorker	(std::string, int);
	int  stopWorker		(std::string);
	void stopAllWorker	(void);
	//void procMonitorCmd(MonitorRequest);
	ExeTask*		getExeTask(std::string tName);
	TaskManager*	getTaskManager();

	void list();
	void showRunningTasks();
	void showDeployedTasks();
	void showDeployedBehaviors();
	void displayInfo(char* head, std::string msg, char* end);

	std::string getRunningWorkerList(void);
	std::string getLoadedWorkerList	(void);

	
	//debugging
	void setAsDebugMode();

	void run();
};
