// Copyright (c) 2020 by Bug
//
// file  : TCPServer.h
// since : 2020-7-31
// desc  : windows网络编程-阻塞模式 tcp编程类 


#ifndef __TCPSERVER__
#define __TCPSERVER__

#include <WinSock2.h>
#include <iostream>
#include <string>

#define MAX_BUFFER_LENG	1024


class TCPServer
{
private:
	SOCKET		m_socket;
	int			m_port;

public:
	TCPServer();
	TCPServer(int port);
	~TCPServer();

	bool startUp();

	void releaseSocket();
};


#endif // !__TCPSERVER__



