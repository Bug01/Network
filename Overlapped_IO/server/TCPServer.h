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

	// �ͷ�
	void releaseSocket();
public:
	TCPServer();
	~TCPServer();

	static TCPServer* m_this;

	// �ص�I/O���¼�֪ͨ����Ҫ��windows�¼�������WSAOVERLAPPED�ṹ������һ����ʹ��
	// һ��WSAOVERLAPPED�ṹ��������WSASend��WSARecv������I/O���ã����ǻ��������ء�

	// �ص�I/O - �¼�֪ͨ �����汾���ð汾ÿ��ֻ�ܷ���һ���ͻ��ˣ�
	bool startServerEvent_base();

	// �ص�I/O - �¼�֪ͨ
	bool startServerEvent();

	// �ص�I/O - �¼�֪ͨ �ص�I/O�����߳�
	static void startServerEvent_recvThread();


	// ���������ʵ����һЩ���������ǽ���Щ�������ݸ��ص�I/O�����Թ��ص�I/O�������ʱ��ϵͳ���á�

	// �ص�I/O - ������� �����汾���ð汾ÿ��ֻ�ܷ���һ���ͻ��ˣ�
	bool startServerRoutine_base();

	bool startServerRoutine();

	static void startServerRoutine_overThread();

	static void CALLBACK startServerRoutine_recvCallBack(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags);

	
};


#endif // !__TCP_SERVER__



