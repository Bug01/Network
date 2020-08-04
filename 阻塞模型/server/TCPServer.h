// Copyright (c) 2020 by Bug
//
// file  : TCPServer.h
// since : 2020-8-4
// desc  : windows������-����ģʽ tcp_server��TCPServer�ڳ�ʼ��ʱָ���ǵ��߳�ģʽ���Ƕ��߳�ģʽ



// ���Ӻ궨�� _WINSOCK_DEPRECATED_NO_WARNINGS

#ifndef __TCP_SERVER__
#define __TCP_SERVER__

#include <WinSock2.h>
#include <iostream>
#include <string>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

#define MAX_BUFFER_LENG	1024
#define SERVER_PORT 8889

// ������ģʽ
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

	// ����������
	bool startServer();

	static void dealClient(SOCKET client, sockaddr_in cliAddr);
};


#endif // !__TCP_SERVER__



