// Copyright (c) 2020 by Bug
//
// file  : TCPClient.h
// since : 2020-7-31
// desc  : windowsÍøÂç±à³Ì-×èÈûÄ£Ê½ tcp_client

#ifndef __TCP__CLIENT__
#define __TCP__CLIENT__

#include <WinSock2.h>
#include <iostream>
#include <string>

#pragma comment(lib, "ws2_32.lib")

class TCPClient
{
private:
	SOCKET m_client;

public:
	TCPClient();
	~TCPClient();

	bool loginToServer(std::string ip, int port);

	bool sendToServer(std::string str);

	void releaseSocket();
};

#endif // !__TCP__CLIENT__



