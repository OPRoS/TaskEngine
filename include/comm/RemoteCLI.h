#pragma once

#include "comm/RSockServer.h" //�� include�� ���� ���� �;��Ѵ�. (Ȥ �ٸ� ������ windows.h�� ���� include�� �� �ֱ� ������)
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

	//����͸�
	//MonitorRequest* recvMonReq();
	void commandHelp();
	
	//virtual void cbackBy_Thrfunc();

	//virtual int stop();

	//[���� Ŭ���̾�Ʈ & �̱� ���� ����] ����
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
