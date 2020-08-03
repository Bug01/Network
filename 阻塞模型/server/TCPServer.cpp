#include "TCPServer.h"

TCPServer::TCPServer()
{
}


TCPServer::~TCPServer()
{
}

bool TCPServer::startServer()
{
	WSADATA data;
	int errcode;
	
	// winsock �����ʼ��
	errcode = WSAStartup(MAKEWORD(2, 2), &data);
	if (errcode != 0)
	{
		std::cout << "TCPServer::WSAStartup err.\n";
		return false;
	}

	// ����socket
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
	{
		std::cout << "TCPServer create socket err.\n";
		return false;
	}

	// ���׽���
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SERVER_PORT);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);
	errcode = bind(m_socket, (SOCKADDR*)&addr, sizeof(addr));
	if (errcode == SOCKET_ERROR)
	{
		std::cout << "TCPServer bind socket err.\n";
		releaseSocket();
		return false;
	}

	// �����׽���
	errcode = listen(m_socket, 1);
	if (errcode == SOCKET_ERROR)
	{
		std::cout << "TCPServer listen socket err.\n";
		releaseSocket();
		return false;
	}

	std::cout << "server start!\n";

	sockaddr_in cliAddr;
	int len = sizeof(cliAddr);
	while (true)
	{
		// �ȴ�����
		SOCKET client = accept(m_socket, (SOCKADDR*)&cliAddr, &len);
		if (client == INVALID_SOCKET)
		{
			std::cout << "TCPServer accept socket err.\n";
			break;
		}

		// ��������
		char recvBuff[MAX_BUFFER_LENG];

		int recvLength = recv(client, recvBuff, MAX_BUFFER_LENG, 0);


		if (recvLength > 0)
			std::cout << inet_ntoa(cliAddr.sin_addr) << ": " << recvBuff << std::endl;
		else
			std::cout << "have client\n";

		// �رտͻ�������
		// closesocket(client);
	}

	// �ͷ� socket
	releaseSocket();
	return true;
}

void TCPServer::releaseSocket()
{
	closesocket(m_socket);
	WSACleanup();
}
