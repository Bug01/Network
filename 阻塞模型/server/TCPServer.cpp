#include "TCPServer.h"

TCPServer::TCPServer(SERVER_MODEL model)
{
	m_model = model;
}


TCPServer::~TCPServer()
{
}

// 单线程服务器启动
bool TCPServer::startServer()
{
	// winsock 服务初始化
	WSADATA data;
	int errcode;
	errcode = WSAStartup(MAKEWORD(2, 2), &data);
	if (errcode != 0)
	{
		std::cout << "TCPServer: WSAStartup err.\n";
		return false;
	}

	// 创建socket
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
	{
		std::cout << "TCPServer: create socket err.\n";
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
		std::cout << "TCPServer: bind socket err.\n";
		releaseSocket();
		return false;
	}

	// 监听套接字
	errcode = listen(m_socket, 1);
	if (errcode == SOCKET_ERROR)
	{
		std::cout << "TCPServer: listen socket err.\n";
		releaseSocket();
		return false;
	}

	std::cout << "TCPServer: start Server!\n";

	// 等待连接
	sockaddr_in cliAddr;
	int len = sizeof(cliAddr);
	while (true)
	{
		// 新客户端连接
		SOCKET client = accept(m_socket, (SOCKADDR*)&cliAddr, &len);

		if (client == INVALID_SOCKET)
		{
			std::cout << "TCPServer: accept socket err.\n";
			break;
		}
		else
		{
			std::cout << "TCPServer: new client " << inet_ntoa(cliAddr.sin_addr) << ":" << cliAddr.sin_port << " connect!\n";

			std::string k = "服务器开始接受数据\n";
			send(client, k.c_str(), k.size() + 1, 0);
		}

		// 单线程模式 - 该模式下服务器一次只能服务一个客户端
		if (m_model == SERVER_MODEL::SINGLE_THREAD)
		{
			// 建立连接后进行通信
			while (true)
			{
				// 接收数据
				char recvBuff[MAX_BUFFER_LENG];
				if (recv(client, recvBuff, MAX_BUFFER_LENG, 0) > 0)
				{
					std::cout << inet_ntoa(cliAddr.sin_addr) << ":" << cliAddr.sin_port << ":";
					std::cout << recvBuff << std::endl;

					std::string k1 = "服务器接收数据成功\n";
					send(client, k1.c_str(), k1.size() + 1, 0);
				}
				// 关闭客户端连接
				else
				{
					std::cout << "TCPServer: client " << inet_ntoa(cliAddr.sin_addr) << ":" << cliAddr.sin_port << " disconnect!\n";
					closesocket(client);
					break;
				}
			}
		}
		// 多线程模式
		else
		{
			std::thread t(dealClient, client, cliAddr);
			t.detach();
		}
	}

	// 释放 socket
	releaseSocket();
	return true;
}

void TCPServer::dealClient(SOCKET client, sockaddr_in cliAddr)
{
	while (true)
	{
		// 接收数据
		char recvBuff[MAX_BUFFER_LENG];
		if (recv(client, recvBuff, MAX_BUFFER_LENG, 0) > 0)
		{
			std::cout << inet_ntoa(cliAddr.sin_addr) << ":" << cliAddr.sin_port << ":";
			std::cout << recvBuff << std::endl;

			std::string k1 = "服务器接收数据成功\n";
			send(client, k1.c_str(), k1.size() + 1, 0);
		}
		// 关闭客户端连接
		else
		{
			std::cout << "TCPServer: client " << inet_ntoa(cliAddr.sin_addr) << ":" << cliAddr.sin_port << " disconnect!\n";
			closesocket(client);
			return;
		}
	}
}


void TCPServer::releaseSocket()
{
	closesocket(m_socket);
	WSACleanup();
}
