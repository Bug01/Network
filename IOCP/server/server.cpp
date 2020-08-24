// server.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//


#include "TCPServer.h"
#include <crtdbg.h>

int main()
{
    std::cout << "IOCP server!\n";

	TCPServer server;

	server.startServer();


	// 退出前查看内存是否有泄漏
	_CrtDumpMemoryLeaks();
	system("pause");

	return 0;
}

