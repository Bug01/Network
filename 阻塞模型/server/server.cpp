/* Copyright (c) 2020 by Bug
 file  : server.cpp
 since : 2020-7-31
 desc  : windows网络编程-阻塞模式 服务器代码
		项目需要引入 winsock2.h 和 ws2_32.lib
*/

#include <iostream>
#include "TCPServer.h"

// https://blog.csdn.net/winnie_rq/article/details/52177489

int main()
{
	TCPServer server;
	server.startUp();

    std::cout << "server\n"; 
}

