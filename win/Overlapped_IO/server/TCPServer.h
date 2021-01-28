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
	WSAOVERLAPPED	m_clientOver;
	SOCKET			m_client;
	WSABUF			m_dataBuf;

	bool			m_bPostRecv;
	int				m_index;
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

	static TCPServer* m_this;

	// 重叠I/O的事件通知方法要求将windows事件对象与WSAOVERLAPPED结构关联在一起，若使用
	// 一个WSAOVERLAPPED结构，发出像WSASend和WSARecv这样的I/O调用，它们会立即返回。

	// 重叠I/O - 事件通知 基础版本（该版本每次只能服务一个客户端）
	bool startServerEvent_base();

	// 重叠I/O - 事件通知
	bool startServerEvent();

	// 重叠I/O - 事件通知 重叠I/O处理线程
	static void startServerEvent_recvThread();


	// 完成例程其实就是一些函数，我们将这些函数传递给重叠I/O请求，以供重叠I/O请求完成时由系统调用。

	// 重叠I/O - 完成例程 基础版本（该版本每次只能服务一个客户端）
	bool startServerRoutine_base();

	bool startServerRoutine();

	static void startServerRoutine_overThread();

	static void CALLBACK startServerRoutine_recvCallBack(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags);

	
};


#endif // !__TCP_SERVER__



