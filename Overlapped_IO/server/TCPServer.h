// Copyright (c) 2020 by Bug
//
// file  : TCPServer.h
// since : 2020-8-7
// desc  : windows������- Overlapped I/O tcp_server

#ifndef __TCP_SERVER__
#define __TCP_SERVER__
#include <WinSock2.h>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")


#define SERVER_PORT  8999
#define MAX_BUFFER_LENG	1024

class TCPServer
{
private:
	SOCKET m_server;

	void releaseSocket();
public:
	TCPServer();
	~TCPServer();

	// �ص�I/O - �¼�֪ͨ
	bool StartServerEvent();
};


#endif // !__TCP_SERVER__



