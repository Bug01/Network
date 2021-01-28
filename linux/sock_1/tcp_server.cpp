#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

// 缓冲区长度
#define MAX_BUF_LENG 1024

int main()
{
    int sock_listen, sock_cli;
    struct sockaddr_in addr_svr;
    struct sockaddr_in addr_cli;

    char recv_buf[MAX_BUF_LENG];
    int len;

    // 初始化服务器地址
    memset(&addr_svr, 0, sizeof(addr_svr));
    addr_svr.sin_family = AF_INET;
    addr_svr.sin_addr.s_addr = INADDR_ANY;
    addr_svr.sin_port = htons(9000);

    // 创建tcp套接字
    sock_listen = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_listen < 0)
    {
        perror("create socket error.");
        return 0;
    }

    if (bind(sock_listen, (sockaddr *)&addr_svr, sizeof(addr_svr)) < 0)
    {
        perror("bind socket error.");
        return 0;
    }

    if (listen(sock_listen, 5) < 0)
    {
        perror("listen socket error.");
        return 0;
    }

    int cli_len = sizeof(addr_cli);
    sock_cli = accept(sock_listen, (sockaddr *)&addr_cli, (socklen_t *)&cli_len);
    if (sock_cli < 0)
    {
        perror("accept socket error.");
        return 0;
    }

    len = recv(sock_cli, recv_buf, MAX_BUF_LENG, 0);
    while (len > 0)
    {
        printf("%s\n", recv_buf);

        len = send(sock_cli, recv_buf, len, 0);
        if (len < 0)
        {
            perror("send socket error.");
            return 0;
        }

        len = recv(sock_cli, &recv_buf, MAX_BUF_LENG, 0);
    }

    close(sock_cli);
    close(sock_listen);

    return 0;
}