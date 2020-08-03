/*
 Copyright (c) 2020 by Bug
 file  : server.cpp
 since : 2020-7-31
 desc  : windows网络编程-阻塞模式 服务器代码
*/

#include <iostream>
#include "TCPServer.h"

int main()
{
	TCPServer server;
	server.startServer();
}

