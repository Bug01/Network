// Copyright (c) 2020 by Bug
//
// file  : TCPServer.h
// since : 2020-8-14
// desc  : windowsÍøÂç±à³Ì- IOCP tcp_server

#ifndef __TCP_SERVER__
#define __TCP_SERVER__

#include <iostream>
#include <WinSock2.h>
#include <thread>
#include <vector>
#pragma comment(lib, "ws2_32.lib")

#define SERVER_PORT  8889
#define MAX_BUFFER_LENG	1024

struct CClientAddr
{
	SOCKET		m_client;
	sockaddr_in	m_addr;
};

struct CClientData
{
	OVERLAPPED	m_clientOver;
	WSABUF		m_dataBuff;
};

class TCPServer
{
private:
	SOCKET m_server;

	void releaseSocket();
public:
	TCPServer();
	~TCPServer();

	bool startServer();

	static void wordComplatePoint(HANDLE complatePoint);

	static TCPServer* m_this;
};

#endif // !__TCP_SERVER__





