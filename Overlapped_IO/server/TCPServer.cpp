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

	// �����ص�ģ�� socket
	m_server = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (m_server == INVALID_SOCKET)
	{
		std::cout << "TCPServer WSASocket err.\n";
		return ;
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
		return;
	}

	// ����
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
		// ��������
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

			std::string k = "���ӷ������ɹ�!\n";
			send(clientSocket, k.c_str(), k.size() + 1, 0);
		}

		// ����һ���¼�����
		WSAEVENT clientEvent = WSACreateEvent();

		// �����ص��ṹ ���ҹ����¼�����
		WSAOVERLAPPED clientOver;
		memset(&clientOver, 0, sizeof(clientOver));
		clientOver.hEvent = clientEvent;

		// ���ݽ���
		WSABUF clientDataBuff;
		char buff[MAX_BUFFER_LENG];
		clientDataBuff.len = MAX_BUFFER_LENG;
		clientDataBuff.buf = buff;
		DWORD dwRecvBytes = 0;
		DWORD dwFlags = 0;

		// Ͷ��һ�� WSARecv��Ȼ����socket�Ͻ�������
		if (WSARecv(clientSocket, &clientDataBuff, 1, &dwRecvBytes, &dwFlags, &clientOver, NULL) == SOCKET_ERROR)
		{
			// WSA_IO_PENDING ��ʾI/O�������ڽ���
			if (WSAGetLastError() != WSA_IO_PENDING)
			{
				std::cout << "TCPServer WSARecv err.\n";
				return false;
			}
		}

		// �����׽����ϵ��ص�����
		while (true)
		{
			DWORD index = WSAWaitForMultipleEvents(1, &clientEvent, FALSE, WSA_INFINITE, FALSE);

			if (index == WSA_WAIT_TIMEOUT || index == WSA_WAIT_FAILED)
				continue;

			// �����¼�
			WSAResetEvent(clientEvent);

			DWORD byteTransferred;
			WSAGetOverlappedResult(clientSocket, &clientOver, &byteTransferred, FALSE, &dwFlags);

			// ����Ͽ�����
			if (byteTransferred == 0)
			{
				std::cout << "TCPServer: client disconnect!\n";
				closesocket(clientSocket);
				WSACloseEvent(clientEvent);

				break;
			}
			// ��������
			else
			{
				std::cout << clientDataBuff.buf << std::endl;

				std::string k = "�������յ���Ϣ!\n";
				send(clientSocket, k.c_str(), k.size() + 1, 0);

				memset(&clientOver, 0, sizeof(clientOver));
				clientOver.hEvent = clientEvent;

				clientDataBuff.len = MAX_BUFFER_LENG;
				clientDataBuff.buf = buff;


				// Ͷ��һ�� WSARecv��Ȼ����socket�Ͻ�������
				if (WSARecv(clientSocket, &clientDataBuff, 1, &dwRecvBytes, &dwFlags, &clientOver, NULL) == SOCKET_ERROR)
				{
					// WSA_IO_PENDING ��ʾI/O�������ڽ���
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
	// �������߳�
	std::thread t(startServerEvent_recvThread);
	t.detach();

	// �ȴ�����
	sockaddr_in cliAddr;
	int len = sizeof(cliAddr);
	while (true)
	{
		// ��������
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

			std::string k = "���ӷ������ɹ�!\n";
			send(client, k.c_str(), k.size() + 1, 0);
		}
		
		// �����¼�
		m_clientEvent[m_clientCount] = WSACreateEvent();

		// �����ص��ṹ
		char buff[MAX_BUFFER_LENG];
		clientOLData* clientData = new clientOLData();

		clientData->m_client = client;
		clientData->m_dataBuf.len = MAX_BUFFER_LENG;
		clientData->m_dataBuf.buf = buff;
		clientData->m_clientOver.hEvent = m_clientEvent[m_clientCount];
		m_clientData[m_clientCount++] = clientData;
		
		// Ͷ��һ�� WSARecv��Ȼ����socket�Ͻ�������
		DWORD dwFlags = 0;
		DWORD dwRecvBytes = 0;
		if (WSARecv(clientData->m_client, &clientData->m_dataBuf, 1, &dwRecvBytes, &dwFlags, &clientData->m_clientOver, NULL) == SOCKET_ERROR)
		{
			// WSA_IO_PENDING ��ʾI/O�������ڽ���
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
		// �ȴ��ص��������
		// ����1���Ⱥ��¼���������
		// ����2���¼������ָ��
		// ����3���������Ϊ TRUE�����¼������������¼������ŵ�ʱ�����Ż᷵�أ�FALSE���κ�һ���¼������ź�����Ҫ����
		// ����4����ʱʱ�䣬�����ʱ�������᷵��WSA_WAIT_TIMEOUT���������Ϊ0���������������أ��������Ϊ WSA_INFINITEֻ����ĳһ���¼������ź�Ż᷵��
		// ����5������������л��õ��������
		// ���أ��¼���������ĳһ���¼��������ˣ������᷵������¼�������ֵ�������������ֵ��Ҫ��ȥԤ����ֵ WSA_WAIT_EVENT_0��������¼����¼������е�λ�á�
		DWORD index = WSAWaitForMultipleEvents(m_this->m_clientCount, m_this->m_clientEvent, FALSE, 1000, FALSE);
		if (index == WSA_WAIT_TIMEOUT || index == WSA_WAIT_FAILED)
			continue;

		// �����¼�
		index -= WSA_WAIT_EVENT_0;
		WSAResetEvent(m_this->m_clientEvent[index]);

		// ����ѯһ���ص������Ľ��
		// ����1��SOCKET
		// ����2����Ҫ��ѯ������Ǹ��ص��ṹ��ָ��
		// ����3�������ص�������ʵ�ʽ���(����)���ֽ���������ֽ���Ϊ0��ʾͨ�ŶԷ��Ѿ��ر�����
		// ����4������ΪTRUE�������ص�������ɣ����������᷵�أ�����FALSE���Ҳ����Դ��ڹ���״̬����ô�����ͻ᷵��FALSE
		// ����5��������ս����־
		DWORD dwFlags;
		DWORD byteTransferred;
		clientOLData* clientData = m_this->m_clientData[index];
		WSAGetOverlappedResult(clientData->m_client, &(clientData->m_clientOver), &byteTransferred, FALSE, &dwFlags);
		
		// ����Ͽ�����
		if (byteTransferred == 0)
		{
			std::cout << "TCPServer client:" << index << " disconnect!\n";

			// �ر�����
			m_this->m_clientCount--;
			closesocket(clientData->m_client);
			WSACloseEvent(m_this->m_clientEvent[index]);
			delete m_this->m_clientData[index];

			// ɾ������
			if (index < m_this->m_clientCount)
			{
				m_this->m_clientData[index] = m_this->m_clientData[m_this->m_clientCount];
				m_this->m_clientEvent[index] = m_this->m_clientEvent[m_this->m_clientCount];
			}
			m_this->m_clientEvent[m_this->m_clientCount] = NULL;
		}
		// ��������
		else
		{
			std::cout << "index:" << index << " " << clientData->m_dataBuf.buf << std::endl;

			std::string k = "�������յ���Ϣ!\n";
			send(clientData->m_client, k.c_str(), k.size() + 1, 0);

			// �������¼������ص��ṹ
			memset(&clientData->m_clientOver, 0, sizeof(clientData->m_clientOver));
			clientData->m_clientOver.hEvent = m_this->m_clientEvent[index];

			char buff[MAX_BUFFER_LENG];
			clientData->m_dataBuf.len = MAX_BUFFER_LENG;
			clientData->m_dataBuf.buf = buff;

			// Ͷ��һ�� WSARecv��Ȼ����socket�Ͻ�������
			DWORD dwRecvBytes = 0;
			if (WSARecv(clientData->m_client, &clientData->m_dataBuf, 1, &dwRecvBytes, &dwFlags, &clientData->m_clientOver, NULL) == SOCKET_ERROR)
			{
				// WSA_IO_PENDING ��ʾI/O�������ڽ���
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
	// ��������
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

		std::string k = "���ӷ������ɹ�!\n";
		send(clientSocket, k.c_str(), k.size() + 1, 0);
	}

	// �����ص��ṹ
	WSAOVERLAPPED clientOver;
	memset(&clientOver, 0, sizeof(clientOver));
	m_clientEvent[0] = WSACreateEvent();

	// ���ݽ���
	WSABUF clientDataBuff;
	char buff[MAX_BUFFER_LENG];
	clientDataBuff.len = MAX_BUFFER_LENG;
	clientDataBuff.buf = buff;
	DWORD dwRecvBytes = 0;
	DWORD dwFlags = 0;

	// Ͷ��һ�� WSARecv��Ȼ����socket�Ͻ�������
	if (WSARecv(clientSocket, &clientDataBuff, 1, &dwRecvBytes, &dwFlags, &clientOver, startServerRoutine_recvCallBack) == SOCKET_ERROR)
	{
		// WSA_IO_PENDING ��ʾI/O�������ڽ���
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			std::cout << "TCPServer WSARecv err.\n";
			return false;
		}
	}

	// �����׽����ϵ��ص�����
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

	// Ͷ��һ�� WSARecv��Ȼ����socket�Ͻ�������
	if (WSARecv(m_this->m_clientSocket, &clientDataBuff, 1, &RecvBytes, &Flags, lpOverlapped, startServerRoutine_recvCallBack) == SOCKET_ERROR)
	{
		// WSA_IO_PENDING ��ʾI/O�������ڽ���
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			std::cout << "TCPServer WSARecv err.\n";
			return;
		}
	}
}
