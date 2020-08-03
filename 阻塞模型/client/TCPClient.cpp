#include "TCPClient.h"


TCPClient::TCPClient()
{
}

TCPClient::~TCPClient()
{
}

bool TCPClient::loginToServer(std::string serverIP, int serverPort)
{
	WSADATA data;
	int errcode;
	// winsock 初始化
	errcode = WSAStartup(MAKEWORD(2, 2), &data);
	if (errcode != 0)
	{
		std::cout << "TCPClient::WSAStartup err.\n";
		return false;
	}

	// 创建socket
	m_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_client == INVALID_SOCKET)
	{
		std::cout << "TCPClient create socket err.\n";
		return false;
	}

	// 连接服务器
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(serverPort);
	addr.sin_addr.S_un.S_addr = inet_addr(serverIP.c_str());

	errcode = connect(m_client, (SOCKADDR*)&addr, sizeof(addr));
	if (errcode != 0)
	{
		std::cout << "TCPClient connect to server err.";
		releaseSocket();
		return false;
	}

	return true;
}

bool TCPClient::sendToServer(std::string str)
{
	int sendLength = send(m_client, str.c_str(), str.size() + 1, 0);

	if (sendLength != str.size() + 1)
		return false;
	else
		std::cout << "send successful!\n";

	return true;
}

void TCPClient::releaseSocket()
{
	closesocket(m_client);
	WSACleanup();
}


