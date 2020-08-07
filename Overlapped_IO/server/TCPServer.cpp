#include "TCPServer.h"



TCPServer::TCPServer()
{
}


TCPServer::~TCPServer()
{
}

bool TCPServer::StartServerEvent()
{
	// 初始化环境
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
	{
		std::cout << "TCPServer WSAStartup err.\n";
		return false;
	}

	// 创建重叠模型 socket
	m_server = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_server == INVALID_SOCKET)
	{
		std::cout << "TCPServer WSASocket err.\n";
		return false;
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
		return false;
	}

	// 监听
	if (listen(m_server, 1) == SOCKET_ERROR)
	{
		std::cout << "TCPServer listen err.\n";
		releaseSocket();
		return false;
	}

	// 接收连接
	SOCKET client = accept(m_server, NULL, NULL);
	if (client == INVALID_SOCKET)
	{
		std::cout << "TCPServer accept err.\n";
		releaseSocket();
		return false;
	}

	WSAEVENT eventArray[WSA_MAXIMUM_WAIT_EVENTS];
	eventArray[0] = WSACreateEvent();
	
	WSAOVERLAPPED clientOverlp;
	memset(&clientOverlp, 0, sizeof(clientOverlp));
	clientOverlp.hEvent = eventArray[0];

	DWORD recvBytes;
	DWORD flag;
	WSABUF dataBuff;
	char buff[MAX_BUFFER_LENG];
	dataBuff.len = MAX_BUFFER_LENG;
	dataBuff.buf = buff;

	
	WSARecv(m_server, &dataBuff, 1, &recvBytes, &flag, &clientOverlp, NULL);

	return true;
}


void TCPServer::releaseSocket()
{
	closesocket(m_server);
	WSACleanup();
}