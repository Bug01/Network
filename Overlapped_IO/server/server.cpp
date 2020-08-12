// server.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "TCPServer.h"
#include <crtdbg.h>

int main()
{
	std::cout << "Overlapped_IO server!\n";

	TCPServer server;
	//server.startServerEvent_base();
	//server.startServerEvent();
	server.startServerRoutine();

	return 0;
}
