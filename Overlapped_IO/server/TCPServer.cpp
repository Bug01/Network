#include "TCPServer.h"


TCPServer* TCPServer::m_this = nullptr;

TCPServer::TCPServer()
{
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
	{
		std::cout << "TCPServer WSAStartup err.\n";
		return;
	}

	// 创建重叠模型 socket
	m_server = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_server == INVALID_SOCKET)
	{
		std::cout << "TCPServer WSASocket err.\n";
		return ;
	}

	// 绑定端口
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SERVER_PORT);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);
	if (bind(m_server, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		std::cout << "TCPServer bind err.\n";
		releaseSocket();
		return;
	}

	// 监听
	if (listen(m_server, 1) == SOCKET_ERROR)
	{
		std::cout << "TCPServer listen err.\n";
		releaseSocket();
		return;
	}

	m_this = this;
}


TCPServer::~TCPServer()
{
	for (DWORD i = 0; i < m_clientCount; i++)
	{
		delete m_clientData[i];
		m_clientData[i] = NULL;
	}
}


void TCPServer::releaseSocket()
{
	closesocket(m_server);
	WSACleanup();
}



bool TCPServer::startServerEvent_base()
{
	while (true)
	{
		// 接收连接
		sockaddr_in cliAddr;
		int len = sizeof(cliAddr);
		SOCKET clientSocket = accept(m_server, (LPSOCKADDR)&cliAddr, &len);
		if (clientSocket == INVALID_SOCKET)
		{
			std::cout << "TCPServer accept err.\n";
			releaseSocket();
			return false;
		}
		else
		{
			std::cout << "TCPServer: new client " << inet_ntoa(cliAddr.sin_addr) << ":" << cliAddr.sin_port << " connect!\n";

			std::string k = "连接服务器成功!\n";
			send(clientSocket, k.c_str(), k.size() + 1, 0);
		}

		// 创建一个事件对象
		WSAEVENT clientEvent = WSACreateEvent();

		// 建立重叠结构 并且关联事件对象
		WSAOVERLAPPED clientOver;
		memset(&clientOver, 0, sizeof(clientOver));
		clientOver.hEvent = clientEvent;

		// 数据接收
		WSABUF clientDataBuff;
		char buff[MAX_BUFFER_LENG];
		clientDataBuff.len = MAX_BUFFER_LENG;
		clientDataBuff.buf = buff;
		DWORD dwRecvBytes = 0;
		DWORD dwFlags = 0;

		// 投递一个 WSARecv，然后再socket上接收数据
		if (WSARecv(clientSocket, &clientDataBuff, 1, &dwRecvBytes, &dwFlags, &clientOver, NULL) == SOCKET_ERROR)
		{
			// WSA_IO_PENDING 表示I/O操作正在进行
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				std::cout << "TCPServer WSARecv err.\n";
				return false;
			}
		}

		// 处理套接字上的重叠接收
		while (true)
		{
			DWORD index = WSAWaitForMultipleEvents(1, &clientEvent, FALSE, WSA_INFINITE, FALSE);

			if (index == WSA_WAIT_TIMEOUT || index == WSA_WAIT_FAILED)
				continue;

			// 重置事件
			WSAResetEvent(clientEvent);

			DWORD byteTransferred;
			WSAGetOverlappedResult(clientSocket, &clientOver, &byteTransferred, FALSE, &dwFlags);

			// 处理断开连接
			if (byteTransferred == 0)
			{
				std::cout << "TCPServer: client disconnect!\n";
				closesocket(clientSocket);
				WSACloseEvent(clientEvent);

				break;
			}
			// 处理数据
			else
			{
				std::cout << clientDataBuff.buf << std::endl;

				std::string k = "服务器收到消息!\n";
				send(clientSocket, k.c_str(), k.size() + 1, 0);

				memset(&clientOver, 0, sizeof(clientOver));
				clientOver.hEvent = clientEvent;

				clientDataBuff.len = MAX_BUFFER_LENG;
				clientDataBuff.buf = buff;


				// 投递一个 WSARecv，然后再socket上接收数据
				if (WSARecv(clientSocket, &clientDataBuff, 1, &dwRecvBytes, &dwFlags, &clientOver, NULL) == SOCKET_ERROR)
				{
					// WSA_IO_PENDING 表示I/O操作正在进行
					if (WSAGetLastError() != WSA_IO_PENDING)
					{
						std::cout << "TCPServer WSARecv in while err.\n";
						return false;
					}
				}
			}
		}
	}

	releaseSocket();
	return true;
}

bool TCPServer::startServerEvent()
{
	// 创建多线程
	std::thread t(startServerEvent_recvThread);
	t.detach();

	// 等待连接
	sockaddr_in cliAddr;
	int len = sizeof(cliAddr);
	while (true)
	{
		// 接收连接
		SOCKET client = accept(m_server, (SOCKADDR*)&cliAddr, &len);
		if (client == INVALID_SOCKET)
		{
			std::cout << "TCPServer accept err.\n";
			releaseSocket();
			return false;
		}
		else
		{
			std::cout << "TCPServer: new client " << inet_ntoa(cliAddr.sin_addr) << ":" << cliAddr.sin_port << " connect!\n";

			std::string k = "连接服务器成功!\n";
			send(client, k.c_str(), k.size() + 1, 0);
		}
		
		// 创建事件
		m_clientEvent[m_clientCount] = WSACreateEvent();

		// 创建重叠结构
		char buff[MAX_BUFFER_LENG];
		clientOLData* clientData = new clientOLData();

		clientData->m_client = client;
		clientData->m_dataBuf.len = MAX_BUFFER_LENG;
		clientData->m_dataBuf.buf = buff;
		clientData->m_clientOver.hEvent = m_clientEvent[m_clientCount];
		m_clientData[m_clientCount++] = clientData;
		
		// 投递一个 WSARecv，然后再socket上接收数据
		DWORD dwFlags = 0;
		DWORD dwRecvBytes = 0;
		if (WSARecv(clientData->m_client, &clientData->m_dataBuf, 1, &dwRecvBytes, &dwFlags, &clientData->m_clientOver, NULL) == SOCKET_ERROR)
		{
			// WSA_IO_PENDING 表示I/O操作正在进行
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				closesocket(client);
				std::cout << "TCPServer WSARecv in while err.\n";
				return false;
			}
		}
	}

	return true;
}


void TCPServer::startServerEvent_recvThread()
{
	while (true)
	{
		// 等待重叠操作完成
		// 参数1：等候事件的总数量
		// 参数2：事件数组的指针
		// 参数3：如果设置为 TRUE，则事件数组中所有事件被传信的时候函数才会返回，FALSE则任何一个事件被传信函数都要返回
		// 参数4：超时时间，如果超时，函数会返回WSA_WAIT_TIMEOUT，如果设置为0，函数会立即返回，如果设置为 WSA_INFINITE只有在某一个事件被传信后才会返回
		// 参数5：在完成例程中会用到这个参数
		// 返回：事件数组中有某一个事件被传信了，函数会返回这个事件的索引值，但是这个索引值需要减去预定义值 WSA_WAIT_EVENT_0才是这个事件在事件数组中的位置。
		DWORD index = WSAWaitForMultipleEvents(m_this->m_clientCount, m_this->m_clientEvent, FALSE, 1000, FALSE);
		if (index == WSA_WAIT_TIMEOUT || index == WSA_WAIT_FAILED)
			continue;

		// 重置事件
		index -= WSA_WAIT_EVENT_0;
		WSAResetEvent(m_this->m_clientEvent[index]);

		// 来查询一下重叠操作的结果
		// 参数1：SOCKET
		// 参数2：想要查询结果的那个重叠结构的指针
		// 参数3：本次重叠操作的实际接收(或发送)的字节数，如果字节数为0表示通信对方已经关闭连接
		// 参数4：设置为TRUE，除非重叠操作完成，否则函数不会返回，设置FALSE而且操作仍处于挂起状态，那么函数就会返回FALSE
		// 参数5：负责接收结果标志
		DWORD dwFlags;
		DWORD byteTransferred;
		clientOLData* clientData = m_this->m_clientData[index];
		WSAGetOverlappedResult(clientData->m_client, &(clientData->m_clientOver), &byteTransferred, FALSE, &dwFlags);
		
		// 处理断开连接
		if (byteTransferred == 0)
		{
			std::cout << "TCPServer client:" << index << " disconnect!\n";

			// 关闭连接
			m_this->m_clientCount--;
			closesocket(clientData->m_client);
			WSACloseEvent(m_this->m_clientEvent[index]);
			delete m_this->m_clientData[index];

			// 删除数据
			if (index < m_this->m_clientCount)
			{
				m_this->m_clientData[index] = m_this->m_clientData[m_this->m_clientCount];
				m_this->m_clientEvent[index] = m_this->m_clientEvent[m_this->m_clientCount];
			}
			m_this->m_clientEvent[m_this->m_clientCount] = NULL;
		}
		// 处理数据
		else
		{
			std::cout << "index:" << index << " " << clientData->m_dataBuf.buf << std::endl;

			std::string k = "服务器收到消息!\n";
			send(clientData->m_client, k.c_str(), k.size() + 1, 0);

			// 重新让事件关联重叠结构
			memset(&clientData->m_clientOver, 0, sizeof(clientData->m_clientOver));
			clientData->m_clientOver.hEvent = m_this->m_clientEvent[index];

			char buff[MAX_BUFFER_LENG];
			clientData->m_dataBuf.len = MAX_BUFFER_LENG;
			clientData->m_dataBuf.buf = buff;

			// 投递一个 WSARecv，然后再socket上接收数据
			DWORD dwRecvBytes = 0;
			if (WSARecv(clientData->m_client, &clientData->m_dataBuf, 1, &dwRecvBytes, &dwFlags, &clientData->m_clientOver, NULL) == SOCKET_ERROR)
			{
				// WSA_IO_PENDING 表示I/O操作正在进行
				if (WSAGetLastError() != WSA_IO_PENDING)
				{
					std::cout << "TCPServer WSARecv in while err.\n";
					return;
				}
			}

		}
	}
}

bool TCPServer::startServerRoutine()
{
	// 接收连接
	sockaddr_in cliAddr;
	int len = sizeof(cliAddr);
	SOCKET clientSocket = accept(m_server, (LPSOCKADDR)&cliAddr, &len);
	if (clientSocket == INVALID_SOCKET)
	{
		std::cout << "TCPServer accept err.\n";
		releaseSocket();
		return false;
	}
	else
	{
		std::cout << "TCPServer: new client " << inet_ntoa(cliAddr.sin_addr) << ":" << cliAddr.sin_port << " connect!\n";

		std::string k = "连接服务器成功!\n";
		send(clientSocket, k.c_str(), k.size() + 1, 0);
	}

	// 建立重叠结构
	WSAOVERLAPPED clientOver;
	memset(&clientOver, 0, sizeof(clientOver));
	m_clientEvent[0] = WSACreateEvent();

	// 数据接收
	WSABUF clientDataBuff;
	char buff[MAX_BUFFER_LENG];
	clientDataBuff.len = MAX_BUFFER_LENG;
	clientDataBuff.buf = buff;
	DWORD dwRecvBytes = 0;
	DWORD dwFlags = 0;

	// 投递一个 WSARecv，然后再socket上接收数据
	if (WSARecv(clientSocket, &clientDataBuff, 1, &dwRecvBytes, &dwFlags, &clientOver, startServerRoutine_recvCallBack) == SOCKET_ERROR)
	{
		// WSA_IO_PENDING 表示I/O操作正在进行
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			std::cout << "TCPServer WSARecv err.\n";
			return false;
		}
	}

	// 处理套接字上的重叠接收
	while (true)
	{
		DWORD index = WSAWaitForMultipleEvents(1, m_clientEvent, FALSE, WSA_INFINITE, TRUE);
		if (index == WAIT_IO_COMPLETION)
			continue;
		else
		{
			std::cout << "TCPServer WSAWaitForMultipleEvents err.\n";
			return false;
		}
	}

	releaseSocket();
	return true;
}

void CALLBACK TCPServer::startServerRoutine_recvCallBack(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags)
{
	if (dwError != 0 || cbTransferred == 0)
	{
		closesocket(m_this->m_clientSocket);
		return;
	}

	memset(lpOverlapped, 0, sizeof(lpOverlapped));
	WSABUF clientDataBuff;
	char buff[MAX_BUFFER_LENG];
	clientDataBuff.len = MAX_BUFFER_LENG;
	clientDataBuff.buf = buff;

	DWORD RecvBytes = 0;
	DWORD Flags = 0;

	// 投递一个 WSARecv，然后再socket上接收数据
	if (WSARecv(m_this->m_clientSocket, &clientDataBuff, 1, &RecvBytes, &Flags, lpOverlapped, startServerRoutine_recvCallBack) == SOCKET_ERROR)
	{
		// WSA_IO_PENDING 表示I/O操作正在进行
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			std::cout << "TCPServer WSARecv err.\n";
			return;
		}
	}
}
