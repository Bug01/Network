// Copyright (c) 2020 by Bug
//
// file  : TCPServer.h
// since : 2020-7-31
// desc  : windowsÍøÂç±à³Ì-×èÈûÄ£Ê½ tcp_server


#ifndef __TCP_SERVER__
#define __TCP_SERVER__

#include <WinSock2.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

#define MAX_BUFFER_LENG	1024
#define SERVER_PORT 8889


class TCPServer
{
private:
	SOCKET		m_socket;

	void releaseSocket();
public:
	TCPServer();
	~TCPServer();

	bool startServer();
};


#endif // !__TCP_SERVER__



