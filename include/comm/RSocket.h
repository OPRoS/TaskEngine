#ifndef __RSOCKET_H__
#define __RSOCKET_H__


#ifdef _WIN32 // Winsock 구 버전 대신 Winsock 2 API 가 사용되는지 확인
	#ifndef _WINSOCKAPI_
	#define _WINSOCKAPI_
	#endif
#endif

#ifdef _WIN32 // Windows Headers
	#pragma comment (lib, "ws2_32.lib") // Winsock 라이브러리 자동 링크

	//아래 winsock2와 windows inlcude 순서가 매우 중요하다.
	//#define WIN32_LEAN_AND_MEAN
	#include <winsock2.h>
	#include <windows.h>

	#define NET_INVALID_SOCKET	INVALID_SOCKET
	#define NET_SOCKET_ERROR	SOCKET_ERROR
	typedef int socklen_t; // Unix 타입은 unsigned int Windows 는 int

#else // Unix Headers
	#include <sys/socket.h> // for socket(), bind(), connect()
	#include <arpa/inet.h> 	// for sockaddr_in, inet_ntoa()
	#include <unistd.h> 	// for close()
	#include <stdlib.h> 	// for exit()

	typedef int SOCKET;
	#define NET_INVALID_SOCKET	-1
	#define NET_SOCKET_ERROR -1
#endif


#endif