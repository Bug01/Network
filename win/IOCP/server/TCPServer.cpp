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

	m_server = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_server == INVALID_SOCKET)
	{
		std::cout << "TCPServer create socket err.\n";
		return;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SERVER_PORT);
	addr.sin_addr.S_un.S_addr = htonl(INADDR_LOOPBACK);

	if (bind(m_server, (SOCKADDR*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		std::cout << "TCPServer: bind socket err.\n";
		releaseSocket();
		return;
	}

	if (listen(m_server, 1) == SOCKET_ERROR)
	{
		std::cout << "TCPServer: listen socket err.\n";
		releaseSocket();
		return;
	}

	m_this = this;
}

TCPServer::~TCPServer()
{
}

void TCPServer::releaseSocket()
{
	closesocket(m_server);
	WSACleanup();
}


bool TCPServer::startServer()
{
	//  ������ɶ˿�
	HANDLE complatePoint = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);

	// ����cpu���������߳�
	for (DWORD i = 0; i < sysInfo.dwNumberOfProcessors; i++)
	{
		std::thread t(wordComplatePoint, complatePoint);
		t.detach();
	}

	while (true)
	{
		// �ȴ���������
		sockaddr_in sockAddr;
		int len = sizeof(sockAddr);
		SOCKET cliSocket = accept(m_server, (SOCKADDR*)&sockAddr, &len);
		if (cliSocket == INVALID_SOCKET)
		{
			std::cout << "TCPServer accept client err.\n";
			return false;
		}
		else
		{
			std::cout << "TCPServer: new client " << inet_ntoa(sockAddr.sin_addr) << ":" << sockAddr.sin_port << " connect!\n";

			std::string k = "���ӷ������ɹ�!\n";
			send(cliSocket, k.c_str(), k.size() + 1, 0);
		}

		// �����ӵĿͻ����ص��ṹ
		CClientData* cliData = new CClientData();
		memset(cliData, 0, sizeof(CClientData));
		cliData->m_dataBuff.len = MAX_BUFFER_LENG;
		cliData->m_dataBuff.buf = new char[MAX_BUFFER_LENG];

		// �����ӵĿͻ�����Ϣ
		CClientAddr* cliAddr = new CClientAddr();
		cliAddr->m_client = cliSocket;
		memcpy(&cliAddr->m_addr, &sockAddr, sizeof(sockAddr));

		// ����ɶ˿�
		CreateIoCompletionPort((HANDLE)cliSocket, complatePoint, (ULONG_PTR)cliAddr, 0);

		// �ȴ���������
		DWORD dwRecvBytes;
		DWORD dwFlags = 0;
		WSARecv(cliSocket, &cliData->m_dataBuff, 1, &dwRecvBytes, &dwFlags, &cliData->m_clientOver, NULL);
	}

	releaseSocket();
	return true;
}

void TCPServer::wordComplatePoint(HANDLE complatePoint)
{
	while (true)
	{
		OVERLAPPED* cliOverlapped = NULL;
		CClientAddr* cliAddr = NULL;
		DWORD dwByteTransferred;
		GetQueuedCompletionStatus(complatePoint, &dwByteTransferred, (PULONG_PTR)&cliAddr, &cliOverlapped, INFINITE);

		CClientData* cliData = CONTAINING_RECORD(cliOverlapped, CClientData, m_clientOver);
		if (dwByteTransferred == 0)
		{
			std::cout << "TCPServer client disconnect.\n";
			closesocket(cliAddr->m_client);
			delete[] cliData->m_dataBuff.buf;
			delete cliData;
			delete cliAddr;
			return;
		}
		else
		{
			std::cout << cliData->m_dataBuff.buf << std::endl;

			std::string k = "�������յ���Ϣ!\n";
			send(cliAddr->m_client, k.c_str(), k.size() + 1, 0);

			// ���ÿռ�
			memset(&cliData->m_clientOver, 0, sizeof(OVERLAPPED));

			// �ȴ���������
			DWORD dwRecvBytes;
			DWORD dwFlags = 0;
			WSARecv(cliAddr->m_client, &cliData->m_dataBuff, 1, &dwRecvBytes, &dwFlags, &cliData->m_clientOver, NULL);
		}
	}
}
