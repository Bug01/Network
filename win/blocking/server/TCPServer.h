// Copyright (c) 2020 by Bug
//
// file  : TCPServer.h
// since : 2020-8-4
// desc  : windows网络编程-阻塞模式 tcp_server，TCPServer在初始化时指定是单线程模式还是多线程模式



// 增加宏定义 _WINSOCK_DEPRECATED_NO_WARNINGS

#ifndef __TCP_SERVER__
#define __TCP_SERVER__

#include <WinSock2.h>
#include <iostream>
#include <string>
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

	void releaseSocket();
public:
	TCPServer() = default;
	TCPServer(SERVER_MODEL model);
	~TCPServer();

	// 服务器启动
	bool startServer();

	static void dealClient(SOCKET client, sockaddr_in cliAddr);
};


#endif // !__TCP_SERVER__



