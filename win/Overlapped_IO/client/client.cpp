// client.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "TCPClient.h"


int main()
{
    std::cout << "Overlapped_IO client!\n"; 

	TCPClient client;
	if (client.loginToServer("127.0.0.1", 8889))
		std::cout << "finish!\n";
	else
		std::cout << "connect faild!\n";


	Sleep(100000);
	return 0;
}

