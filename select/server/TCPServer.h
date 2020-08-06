#ifndef __TCP_SERVER__
#define __TCP_SERVER__

// Copyright (c) 2020 by Bug
//
// file  : TCPServer.h
// since : 2020-8-5
// desc  : windows网络编程-select模式 tcp_server

#include <WinSock2.h>
#include <iostream>
#include <vector>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

#define MAX_BUFFER_LENG	1024
#define SERVER_PORT 8889

// 服务器模式
enum SERVER_MODEL { SINGLE_THREAD, MULTI_THREAD };

class TCPServer
{
private:
	SOCKET			m_socket;
	SERVER_MODEL	m_model;
	
	std::vector<SOCKET> m_clients;

	void releaseSocket();
public:
	TCPServer() = default;
	TCPServer(SERVER_MODEL model);
	~TCPServer();

	bool startServer();

	bool dealClient(SOCKET s);

	static void worlThread(TCPServer *s);
};


#endif // !__TCP_SERVER__


