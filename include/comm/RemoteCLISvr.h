#pragma once

#include "comm/RSockServer.h" //이 include가 가장 먼저 와야한다. (혹 다른 곳에서 windows.h를 먼저 include할 수 있기 때문에)
#include <iostream>
#include <stdlib.h>
#include <string>
#include "LocalCLI.h"
#include "RemoteCLI.h"
#include "filenavi/OSFileSys.h"
#include <stdio.h>
#include <list>

#ifndef _WIN32
#include <dirent.h>
#endif


class RemoteCLISvr : public RSockServer
{
private:	
	int						BUFSIZE;
	ModelBinder*			m_binder;
	std::list<RemoteCLI*>	m_clientPool;

	int closeOStream(SOCKET clSocket);

public:
	RemoteCLISvr(ModelBinder*, OSFileSys*);
	virtual ~RemoteCLISvr(void);

	RemoteCLI* getRemoteCLI(void);
	virtual void cbackBy_Thrfunc();
	void cmdProc(RemoteCLI*, SOCKET);


	//virtual int stop();

};

