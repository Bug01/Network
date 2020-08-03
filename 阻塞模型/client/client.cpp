/* 
 Copyright (c) 2020 by Bug
 file  : server.cpp
 since : 2020-7-31
 desc  : windows网络编程-阻塞模式 客户端代码
*/

#include <iostream>
#include "TCPClient.h"


int main()
{
	TCPClient client;
	bool bConn;
	std::string str;

	bConn = client.loginToServer("127.0.0.1", 8889);
	if (bConn)
	{
		std::cout << "connect to server successful!\n";

		while (true)
		{
			std::cin >> str;
			client.sendToServer(str);
		}

		client.releaseSocket();
	}
	else
		std::cout << "connect to server failed!\n";
	return 0;
}

