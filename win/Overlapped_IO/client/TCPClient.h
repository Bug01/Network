// Copyright (c) 2020 by Bug
//
// file  : TCPClient.h
// since : 2020-7-31
// desc  : windows������-����ģʽ tcp_client

// ���Ӻ궨�� _WINSOCK_DEPRECATED_NO_WARNINGS

#ifndef __TCP__CLIENT__
#define __TCP__CLIENT__

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

#endif // !__TCP__CLIENT__



