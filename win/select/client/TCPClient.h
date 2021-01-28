#ifndef __TCP_CLIENT__
#define __TCP_CLIENT__

// Copyright (c) 2020 by Bug
//
// file  : TCPClient.h
// since : 2020-8-5
// desc  : windowsÍøÂç±à³Ì-selectÄ£Ê½ tcp_client

#include <WinSock2.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")
#define MAX_BUFFER_LENG	1024

class TCPClient
{
private:
	SOCKET m_client;

public:
	TCPClient();
	~TCPClient();

	bool loginToServer(std::string ip, int port);
};

#endif // !__TCP_CLIENT__



