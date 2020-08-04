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
	if (client.loginToServer("127.0.0.1", 8889))
		std::cout << "finish!\n";
	else
		std::cout << "connect faild!\n";


	Sleep(100000);
	return 0;
}

