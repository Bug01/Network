#include "TCPClient.h"

TCPClient::TCPClient()
{
	// winsock 初始化
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
		std::cout << "TCPClient::WSAStartup err.\n";

	// 创建socket
	m_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (m_client == INVALID_SOCKET)
		std::cout << "TCPClient create socket err.\n";
}

TCPClient::~TCPClient()
{
	closesocket(m_client);
	WSACleanup();
}

bool TCPClient::loginToServer(std::string serverIP, int serverPort)
{
	// 连接服务器
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(serverPort);
	addr.sin_addr.S_un.S_addr = inet_addr(serverIP.c_str());
	if(connect(m_client, (SOCKADDR*)&addr, sizeof(addr)) != 0)
	{
		std::cout << "TCPClient connect to server err.";
		return false;
	}

	std::cout << "connect to server successful!\n";

	// 循环发送数据
	while (true)
	{
		// 接受数据
		char recvBuff[MAX_BUFFER_LENG];
		int recvLength = recv(m_client, recvBuff, MAX_BUFFER_LENG, 0);
		if(recvLength > 0)
			std::cout << "服务器: " << recvBuff << std::endl;

		// 发送数据
		char sendBuff[MAX_BUFFER_LENG];
		std::cin.getline(sendBuff, MAX_BUFFER_LENG);
		send(m_client, sendBuff, MAX_BUFFER_LENG, 0);
	}

	return true;
}




