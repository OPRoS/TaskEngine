/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#ifndef RSOCKSERVER_H_
#define RSOCKSERVER_H_

#include "comm/RSocket.h"
#include "thread/RThread.h"
#include "run/TaskConfig.h"
#include <list>
#include <stdio.h>

class RSockServer : public RThread
{
protected:
	unsigned int		m_connCunt;    //최대 클라이언트 수
	//int				m_curConnNum;
	std::list<SOCKET>	m_clientList;	//클라이언트와 연결 할 소켓
	SOCKET				m_svrSock;		//서버에서 나갈 소켓
	unsigned short		m_port;		

public:
	RSockServer();
	virtual ~RSockServer();

	int		createSocket	(unsigned short);
	int		acceptClient	(void);
	void	closeClient		(SOCKET svrSocket);
	void	closeSocket		(SOCKET svrSocket);
	int		readSocket		(SOCKET clSocket, char* buf, int size);

	/**
	 * \brief writes a message to the client connected 
	 * \param clSocket client's socket
	 * \param buf the message which is transfering to the client's socket
	 * \param size the size of the message
	*/
	void	writeSocket		(SOCKET clSocket, char* buf, int size);
	void	sendMsgWithSize	(SOCKET clSocket, std::string msg, bool withsize);
	//int	closeOStream	(SOCKET clSocket);
	void	error_handling	(char *message) ;
	SOCKET	getRecentClient	(void);	

	//virtual void run()=0;
};

#endif /*RSOCKSERVER_H_*/
