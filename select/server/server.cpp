
/*
 Copyright (c) 2020 by Bug
 file  : server.cpp
 since : 2020-8-5
 desc  : windows网络编程-select模式 函数入口
*/

#include <iostream>
#include "TCPServer.h"

int main()
{
	//TCPServer server(SERVER_MODEL::SINGLE_THREAD);
	TCPServer server(SERVER_MODEL::MULTI_THREAD);
	server.startServer();
}

