#include "TCPServer.h"



TCPServer::TCPServer()
{
}


TCPServer::~TCPServer()
{
}

bool TCPServer::StartServerEvent()
{
	// ��ʼ������
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
	{
		std::cout << "TCPServer WSAStartup err.\n";
		return false;
	}

	// �����ص�ģ�� socket
	m_server = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_server == INVALID_SOCKET)
	{
		std::cout << "TCPServer WSASocket err.\n";
		return false;
	}

	// �󶨶˿�
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

	// ����
	if (listen(m_server, 1) == SOCKET_ERROR)
	{
		std::cout << "TCPServer listen err.\n";
		releaseSocket();
		return false;
	}

	// ��������
	SOCKET client = accept(m_server, NULL, NULL);
	if (client == INVALID_SOCKET)
	{
		std::cout << "TCPServer accept err.\n";
		releaseSocket();
		return false;
	}

	WSAEVENT eventArray;
	WSAOVERLAPPED accOverlp;
	memset(&accOverlp, 0, sizeof(accOverlp));
	accOverlp.hEvent = eventArray;

	return true;
}


void TCPServer::releaseSocket()
{
	closesocket(m_server);
	WSACleanup();
}