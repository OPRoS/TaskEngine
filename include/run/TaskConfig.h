/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2008.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/

#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include "ds/Estring.h"
#include "util/Utility.h"

class TaskConfig
{
private:
	static bool		m_isConfiged;
	static int		m_mode;  //loading 모드
	static int		m_binderMode;
	static bool		m_telsvr;
	static int		m_numCli;
	static int      m_logsize;
	static bool		m_isTaskLogging;
	static bool		m_isThreadTracing;
	static bool		m_autoResultPrint;
	static bool		m_isAutoDeploy;
	static bool		m_modelCodeGen;
	static bool		m_usingQueueTimer;
	static bool		m_loggingTimePeriod;
	//static bool	m_monsvr;

	static unsigned short m_cliport;
	//static unsigned short m_ftpport;
	//static unsigned short m_monitorport;
	

	static std::string m_executorHome;
	static std::string m_deployHome;	//계속 바뀔 수 있음.. 함수 호출시 마다 파일 읽음
	static std::string m_deployFile;
	static std::string m_autoRunTask;

	std::ifstream	m_cfgfile;
	

public:
	TaskConfig();
	virtual ~TaskConfig();

	static const int OVERWRITE	= 1;	//worker, behavior가 이미존재하면 덮어쓰기
	static const int ADDITION	= 2;	//worker, behavior가 이미존재하는건 건너 뜀
	static const int UNIQUE		= 3;	//worker, behavior가 이미 존재하면 에러발생 (loading이 안됨)

	static const int DIRECT		= 1;
	static const int PLUG_IN	= 2;
	static const int COMPONENT	= 3;

	void init();
	
	static int	getBindingMode		(void);
	static int	getDeployMode		(void);
	static int	getLogSize			(void);
	static int	getNumOfCLISession	(void);
	static void setBindingMode		(int);
	static void setDeployMode		(int);
	static bool isTelServerOn		(void);	
	static bool isUsingQueueTimer	(void);
	static bool isloggingTimePeriod	(void);
	static bool getModelCodeGen		(void);
	static bool isAutoDeploy		(void);
	static bool isAutoResultPrint	(void);
	static bool getIsTaskLog		(void);
	static bool getIsThreadTrace	(void);
	//static bool isMonitorSvrOn	(void);
	static bool	beSetted			(void);

	//static std::string getHomeDir		(void);
	static std::string getDeployFile	(void);
	static std::string getAutoRunTask	(void);

	static unsigned short getCLIport	(void);

	static std::string getExecutorHome	(void);
	static std::string getDeployHome	(void);
	static void	setDeployHome	(std::string); //태스크가 실행된 이후 deploy때 변경가능하도록하기 위함
	//static unsigned short getFTPport();
	//static unsigned short getMonitorport();
};

#endif /* CONFIG_H_ */
