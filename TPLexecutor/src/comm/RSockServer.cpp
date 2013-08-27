/*************************************************************************************************
** Project:     OPRoS
** Package:     OPRoS Task Executor
** Date:        2010.10.30
** Author:      Rockwon Kim (rwkim@etri.re.kr)
** Copyright:   Copyright (C) 2010 ETRI
** License:     OPRoS Source Codes License (www.opros.or.kr)
*************************************************************************************************/
#include "comm/RSockServer.h"

RSockServer::RSockServer():RThread("SockSvr", 10)
{
	m_connCunt = TaskConfig::getNumOfCLISession();
//	m_curConnNum = 0;
}

RSockServer:: ~RSockServer()
{
	closeSocket(m_svrSock);
#ifdef _WIN32
	WSACleanup();
#endif
}

int RSockServer::createSocket(unsigned short svrPort)
{
#ifdef _WIN32
	WSADATA wsaData;
	WORD version;
#endif

	struct sockaddr_in ServerAddress; //클라이언트 접속 주소	

#ifdef _WIN32
	version = MAKEWORD(2, 2);
	if(WSAStartup(version, &wsaData) < 0){  //윈속 초기화
		printf("WSAStartup 초기화 에러");
		WSACleanup();
		return -1;
	}
#endif

	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_addr.s_addr = INADDR_ANY;
	ServerAddress.sin_port = htons(svrPort);

	int status = 0;
	m_svrSock = socket(AF_INET, SOCK_STREAM, 0);

	//CREATE
	if(m_svrSock == NET_INVALID_SOCKET)
	{
		printf("Soket creation error\n");
		closeSocket(m_svrSock);
		return -1;
	}

	//BIND
	status = bind(m_svrSock, (struct sockaddr *) &ServerAddress, sizeof(ServerAddress)) ;
	if(status == NET_SOCKET_ERROR){
		printf("Socket bind error\n");
		closeSocket(m_svrSock);
		return -1;
	}

	//LISTEN
	status = listen(m_svrSock, SOMAXCONN);
	if(status == -1)
	{
		printf("Socket listen error\n");
		closeSocket(m_svrSock);
		return -1;
	}

	return 0;
}

int RSockServer::acceptClient()
{
	//ACCEPT
	struct sockaddr_in ClientAddress; //서버 접속 주소
	socklen_t AddressSize = sizeof(ClientAddress);

	SOCKET cliSocket;
	cliSocket = accept(m_svrSock,(struct sockaddr *) &ClientAddress, &AddressSize);
	if(cliSocket == -1){
		printf("Socket accept error\n");
		return -1;
	}
	else{
		if(m_connCunt>m_clientList.size()){
			//client가 여러개면 배열로 만들어서 할당
			m_clientList.push_back(cliSocket);			
		}
		else{
			std::string str = "Connection is refused..\r\n";
			writeSocket(cliSocket, (char*)str.c_str(), str.size());
			closeSocket(cliSocket);//closeClient를 하면 안된다. List에 등록을 하지 않았기 때문
			return -1;
		}
	}
	printf("Connection [SOCKET:%d, IP:%s ,port:%d]\r\n", cliSocket, inet_ntoa(ClientAddress.sin_addr), htons(ClientAddress.sin_port));
	return 0;
}

SOCKET RSockServer::getRecentClient()
{
	std::list<SOCKET>::iterator it= m_clientList.end();
	it--;
	return (*it);
}

void RSockServer::closeClient(SOCKET socket)
{
	closeSocket(socket);
	m_clientList.remove(socket);
}

void RSockServer::closeSocket(SOCKET socket)
{
	//m_curConnNum--;
#ifdef _WIN32
	closesocket(socket);
#else
	close(socket);
#endif
}

int RSockServer::readSocket(SOCKET clSocket, char* buf, int bufsize)
{
#ifdef _WIN32
	return recv(clSocket, buf, bufsize, 0);
#else
	return read(clSocket, buf, bufsize);
#endif

}

/**

*/
void RSockServer::writeSocket(SOCKET clSocket, char* buf, int size)
{
#ifdef _WIN32
	int r = send(clSocket, buf, size, 0);
	//if(r == -1) std::cout<<"====================send error=================="<<std::endl;
	//write(clSocket, buf, size);
#else
	write(clSocket, buf, size);
#endif
}

void RSockServer::error_handling(char *message)
{
  fputs(message, stderr);
  fputc('\n', stderr);
  exit(1);
}

void RSockServer::sendMsgWithSize(SOCKET clSocket, std::string msg, bool withsize)
{
	pthread_mutex_lock(&m_lmu);

	std::string head = "";
	
	//size 전송
	if(withsize){		
		std::string sizeStr = int2strFP(msg.size(), 8);
		head.append(sizeStr);
		//writeSocket(clSocket, (char*)head.c_str(), head.size());
	}
	head.append(msg);

	//Data전송
	writeSocket(clSocket, (char*)head.c_str(), head.size());

	pthread_mutex_unlock(&m_lmu);
}