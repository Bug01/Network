// Copyright (c) 2020 by Bug
//
// file  : TCPServer.h
// since : 2020-8-7
// desc  : windows网络编程- Overlapped I/O tcp_server

#ifndef __TCP_SERVER__
#define __TCP_SERVER__
#include <WinSock2.h>
#include <iostream>
#include <thread>
#include <vector>

#pragma comment(lib, "ws2_32.lib")


#define SERVER_PORT  8889
#define MAX_BUFFER_LENG	1024

struct clientOLData
{
	SOCKET			m_client;
	WSABUF			m_dataBuf;
	WSAOVERLAPPED	m_clientOver;
	
};

class TCPServer
{
private:
	SOCKET			m_server;

	DWORD			m_clientCount = 0;
	WSAEVENT		m_clientEvent[WSA_MAXIMUM_WAIT_EVENTS] = {};
	clientOLData*	m_clientData[WSA_MAXIMUM_WAIT_EVENTS] = {};

	// 释放
	void releaseSocket();
public:
	TCPServer();
	~TCPServer();

	// 重叠I/O - 事件通知 基础版本（该版本每次只能服务一个客户端）
	bool startServerEvent_base();

	// 重叠I/O - 事件通知
	bool startServerEvent();

	// 
	static void dealRecv(TCPServer* s);
};


#endif // !__TCP_SERVER__



