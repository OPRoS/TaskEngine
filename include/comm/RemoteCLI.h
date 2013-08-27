#pragma once

#include "comm/RSockServer.h" //이 include가 가장 먼저 와야한다. (혹 다른 곳에서 windows.h를 먼저 include할 수 있기 때문에)
#include <iostream>
#include <stdlib.h>
#include <string>
#include "LocalCLI.h"
#include "log/Log.h"
#include "filenavi/OSFileSys.h"
//#include "comm/MonitorRequest.h"
#include <stdio.h>
#include "util/Utility.h"
#include "comm/RemoteCLISvr.h"

#ifndef _WIN32
#include <dirent.h>
#endif

class RemoteCLISvr;

class RemoteCLI : public RThread
{
private:
	RSockServer*	m_svrSocket;
	SOCKET			m_cliSocket;
	ModelBinder*	m_binder;
	bool			m_isAvailable;
	bool			m_withsize;
	OSFileSys*		m_fsys;
	//OSFileSys		m_fsys;
	Log				m_log;
	bool			m_prompt;
	bool			m_autoResultPrint;
	LocalCLI*		m_lc;
	//std::string		m_homeDir;
	std::ostringstream m_ostr;

public:
	RemoteCLI(RSockServer*, ModelBinder*, OSFileSys*);
	RemoteCLI(RSockServer*, SOCKET, ModelBinder*, OSFileSys*);
	virtual ~RemoteCLI(void);

	bool isAvailable();
	void setClient(SOCKET);	
	void writeMsg2Sock(std::string msg);
	void writeStream2Sock();
	void writeMsgList2Sock(std::vector<std::string>);	
	
	int procMonitorCmd(LocalCLI*, MonitorRequest);
	MonitorRequest getMonitorReq(unsigned int msgType, std::string attr);

	int recvFile(char* filename, int filesize);
	std::string recvHead();
	int tranFile(std::string home, char *name);
	int tranHead(char* name, unsigned long fileLen);

	std::vector<std::string>	getArgv();
	std::string					getCommand();
	std::string					changeDir2FullPath(std::string dir, bool isdir);	
	std::string					getFnameTPLRoot(std::string);

	//모니터링
	//MonitorRequest* recvMonReq();
	void commandHelp();
	
	//virtual void cbackBy_Thrfunc();

	//virtual int stop();

	//[다중 클라이언트 & 싱글 세션 공유] 지원
	void		init		(void);
	void		cmdProc		(void);
	OSFileSys*	getFileSys	(void);
	LocalCLI*	getLocalCLI	(void);
	std::ostringstream* ostrStream(void);
	void		setAvailable(bool);
	void		setPromptOn	(bool);
	void		setMsgSizeOn(bool);
	bool		isPromptOn();
	Log*		getLog();

};
