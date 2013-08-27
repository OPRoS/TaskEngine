#pragma once

#include "comm/RSockServer.h" //�� include�� ���� ���� �;��Ѵ�. (Ȥ �ٸ� ������ windows.h�� ���� include�� �� �ֱ� ������)
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

