#include "TCPServer.h"

TCPServer::TCPServer(SERVER_MODEL model)
{
	m_model = model;
}


TCPServer::~TCPServer()
{
}

// ���̷߳���������
bool TCPServer::startServer()
{
	// winsock �����ʼ��
	WSADATA data;
	int errcode;
	errcode = WSAStartup(MAKEWORD(2, 2), &data);
	if (errcode != 0)
	{
		std::cout << "TCPServer: WSAStartup err.\n";
		return false;
	}

	// ����socket
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_socket == INVALID_SOCKET)
	{
		std::cout << "TCPServer: create socket err.\n";
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
		std::cout << "TCPServer: bind socket err.\n";
		releaseSocket();
		return false;
	}

	// �����׽���
	errcode = listen(m_socket, 1);
	if (errcode == SOCKET_ERROR)
	{
		std::cout << "TCPServer: listen socket err.\n";
		releaseSocket();
		return false;
	}

	std::cout << "TCPServer: start Server!\n";

	// �ȴ�����
	sockaddr_in cliAddr;
	int len = sizeof(cliAddr);
	while (true)
	{
		// �¿ͻ�������
		SOCKET client = accept(m_socket, (SOCKADDR*)&cliAddr, &len);

		if (client == INVALID_SOCKET)
		{
			std::cout << "TCPServer: accept socket err.\n";
			break;
		}
		else
		{
			std::cout << "TCPServer: new client " << inet_ntoa(cliAddr.sin_addr) << ":" << cliAddr.sin_port << " connect!\n";

			std::string k = "��������ʼ��������\n";
			send(client, k.c_str(), k.size() + 1, 0);
		}

		// ���߳�ģʽ - ��ģʽ�·�����һ��ֻ�ܷ���һ���ͻ���
		if (m_model == SERVER_MODEL::SINGLE_THREAD)
		{
			// �������Ӻ����ͨ��
			while (true)
			{
				// ��������
				char recvBuff[MAX_BUFFER_LENG];
				if (recv(client, recvBuff, MAX_BUFFER_LENG, 0) > 0)
				{
					std::cout << inet_ntoa(cliAddr.sin_addr) << ":" << cliAddr.sin_port << ":";
					std::cout << recvBuff << std::endl;

					std::string k1 = "�������������ݳɹ�\n";
					send(client, k1.c_str(), k1.size() + 1, 0);
				}
				// �رտͻ�������
				else
				{
					std::cout << "TCPServer: client " << inet_ntoa(cliAddr.sin_addr) << ":" << cliAddr.sin_port << " disconnect!\n";
					closesocket(client);
					break;
				}
			}
		}
		// ���߳�ģʽ
		else
		{
			std::thread t(dealClient, client, cliAddr);
			t.detach();
		}
	}

	// �ͷ� socket
	releaseSocket();
	return true;
}

void TCPServer::dealClient(SOCKET client, sockaddr_in cliAddr)
{
	while (true)
	{
		// ��������
		char recvBuff[MAX_BUFFER_LENG];
		if (recv(client, recvBuff, MAX_BUFFER_LENG, 0) > 0)
		{
			std::cout << inet_ntoa(cliAddr.sin_addr) << ":" << cliAddr.sin_port << ":";
			std::cout << recvBuff << std::endl;

			std::string k1 = "�������������ݳɹ�\n";
			send(client, k1.c_str(), k1.size() + 1, 0);
		}
		// �رտͻ�������
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
