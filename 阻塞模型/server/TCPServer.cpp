#include "TCPServer.h"

TCPServer::TCPServer()
{
}


TCPServer::~TCPServer()
{
}

bool TCPServer::startUp()
{
	WSADATA data;
	int errcode;
	
	// winsock 服务初始化
	errcode = WSAStartup(MAKEWORD(2, 2), &data);
	if (errcode != 0)
	{
		std::cout << "TCPServer::WSAStartup err. errcode = " << errcode;
		return false;
	}

	// 创建socket
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
	{
		std::cout << "TCPServer create socket err.";
		return false;
	}

	// 绑定套接字
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SERVER_PORT);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);
	errcode = bind(m_socket, (SOCKADDR*)&addr, sizeof(addr));
	if (errcode == SOCKET_ERROR)
	{
		std::cout << "TCPServer bind socket err.";
		releaseSocket();
		return false;
	}

	// 监听套接字
	errcode = listen(m_socket, 1);
	if (errcode == SOCKET_ERROR)
	{
		std::cout << "TCPServer listen socket err.";
		releaseSocket();
		return false;
	}

	std::cout << "server start!";

	sockaddr_in cliAddr;
	int len = sizeof(cliAddr);
	while (true)
	{
		// 等待连接
		SOCKET client = accept(m_socket, (SOCKADDR*)&cliAddr, &len);
		if (client == INVALID_SOCKET)
		{
			std::cout << "TCPServer accept socket err.";
			break;
		}

		// 接收数据
		char recvBuff[MAX_BUFFER_LENG];
		recv(client, recvBuff, MAX_BUFFER_LENG, 0);
		std::cout << inet_ntoa(cliAddr.sin_addr) << ": " << recvBuff;

		// 关闭客户端连接
		closesocket(client);
	}

	// 释放 socket
	releaseSocket();
	std::cout << "server end!";

	return true;
}

void TCPServer::releaseSocket()
{
	closesocket(m_socket);
	WSACleanup();
}
