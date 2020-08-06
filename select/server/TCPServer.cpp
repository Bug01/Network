#include "TCPServer.h"


TCPServer::TCPServer(SERVER_MODEL model)
{
	m_model = model;
}

TCPServer::~TCPServer()
{
}


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

	std::cout << "TCPServer: start Select Server!\n";

	// 客户端信息
	sockaddr_in cliAddr;
	int len = sizeof(cliAddr);

	if (m_model == SERVER_MODEL::MULTI_THREAD)
	{
		std::thread t(worlThread, this);
		t.detach();

		while (true)
		{
			SOCKET client = accept(m_socket, (sockaddr*)&cliAddr, &len);
			if (client == INVALID_SOCKET)
			{
				std::cout << "TCPServer: accept err.\n";
				break;
			}
			else
			{
				std::cout << "TCPServer: new client " << inet_ntoa(cliAddr.sin_addr) << ":" << cliAddr.sin_port << " connect!\n";

				std::string k = "服务器开始接受数据\n";
				send(client, k.c_str(), k.size() + 1, 0);

				m_clients.push_back(client);
			}
		}
	}
	else
	{
		// 等待连接
		while (true)
		{
			fd_set fdsRead;
			FD_ZERO(&fdsRead);
			FD_SET(m_socket, &fdsRead);

			for (uint32_t i = 0; i < m_clients.size(); i++)
			{
				FD_SET(m_clients[i], &fdsRead);
			}

			int ret = select(0, &fdsRead, NULL, NULL, NULL);
			if (ret < 0)
			{
				std::cout << "TCPServer: select err.\n";
				break;
			}

			if (FD_ISSET(m_socket, &fdsRead))
			{
				FD_CLR(m_socket, &fdsRead);

				// 等待连接
				SOCKET client = accept(m_socket, (sockaddr*)&cliAddr, &len);
				if (client == INVALID_SOCKET)
				{
					std::cout << "TCPServer: accept err.\n";
					break;
				}
				else
				{
					std::cout << "TCPServer: new client " << inet_ntoa(cliAddr.sin_addr) << ":" << cliAddr.sin_port << " connect!\n";

					std::string k = "服务器开始接受数据\n";
					send(client, k.c_str(), k.size() + 1, 0);

					m_clients.push_back(client);
				}
			}

			for (uint32_t i = 0; i < fdsRead.fd_count; i++)
			{
				if (dealClient(fdsRead.fd_array[i]) == false)
				{
					std::vector<SOCKET>::iterator item = std::find(m_clients.begin(), m_clients.end(), fdsRead.fd_array[i]);
					if (item != m_clients.end())
						m_clients.erase(item);
				}
			}
		}
	}
	
	releaseSocket();
	return true;
}

bool TCPServer::dealClient(SOCKET client)
{
	// 接收数据
	char recvBuff[MAX_BUFFER_LENG];
	if (recv(client, recvBuff, MAX_BUFFER_LENG, 0) > 0)
	{
		std::cout << recvBuff << std::endl;

		std::string k1 = "服务器接收数据成功\n";
		send(client, k1.c_str(), k1.size() + 1, 0);
	}
	// 关闭客户端连接
	else
	{
		std::cout << "TCPServer: client disconnect!\n";
		closesocket(client);
		return false;
	}
	
	return true;
}

void TCPServer::worlThread(TCPServer *s)
{
	while (true)
	{
		fd_set fdsRead;
		FD_ZERO(&fdsRead);
		for (uint32_t i = 0; i < s->m_clients.size(); i++)
		{
			FD_SET(s->m_clients[i], &fdsRead);
		}

		timeval tv = { 0,0 };
		int ret = select(0, &fdsRead, NULL, NULL, &tv);

		// 遍历所有read事件
		for (uint32_t i = 0; i < fdsRead.fd_count; i++)
		{
			char recvBuff[MAX_BUFFER_LENG];
			SOCKET client = fdsRead.fd_array[i];

			if (recv(client, recvBuff, MAX_BUFFER_LENG, 0) > 0)
			{
				std::cout << recvBuff << std::endl;

				std::string k1 = "服务器接收数据成功\n";
				send(client, k1.c_str(), k1.size() + 1, 0);
			}
			else
			{
				closesocket(client);

				std::cout << "TCPServer: client disconnect!\n";
				for (std::vector<SOCKET>::iterator it = s->m_clients.begin(); it < s->m_clients.end(); it++)
				{
					if (*it == client)
					{
						s->m_clients.erase(it);
						break;
					}
				}
			}
		}
	}
}


void TCPServer::releaseSocket()
{
	for (uint32_t i = 0; i < m_clients.size(); i++)
	{
		closesocket(m_clients[i]);
	}
	m_clients.clear();
	closesocket(m_socket);
	WSACleanup();
}
