// Copyright (c) 2020 by Bug
//
// file  : TCPServer.h
// since : 2020-8-7
// desc  : windows������- Overlapped I/O tcp_server

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

	// �ͷ�
	void releaseSocket();
public:
	TCPServer();
	~TCPServer();

	// �ص�I/O - �¼�֪ͨ �����汾���ð汾ÿ��ֻ�ܷ���һ���ͻ��ˣ�
	bool startServerEvent_base();

	// �ص�I/O - �¼�֪ͨ
	bool startServerEvent();

	// 
	static void dealRecv(TCPServer* s);
};


#endif // !__TCP_SERVER__



