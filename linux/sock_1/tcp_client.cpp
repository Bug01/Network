#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

// 缓冲区长度
#define MAX_BUF_LENG 1024

int main()
{
    int sock_cli;
    struct sockaddr_in addr_cli;

    char recv_buf[MAX_BUF_LENG];
    int len;

    memset(&addr_cli, 0, sizeof(addr_cli));
    addr_cli.sin_family = AF_INET;
    addr_cli.sin_port = htons(9000);
    addr_cli.sin_addr.s_addr = inet_addr("127.0.0.1");

    sock_cli = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_cli < 0)
    {
        perror("create socket error.");
        return 0;
    }

    if (connect(sock_cli, (sockaddr *)&addr_cli, sizeof(addr_cli)) < 0)
    {
        perror("connect socket error.");
        return 0;
    }

    while (true)
    {
        printf("send msg to server:");
        scanf("%s", recv_buf);

        len = send(sock_cli, recv_buf, sizeof(recv_buf), 0);

        len = recv(sock_cli, recv_buf, MAX_BUF_LENG, 0);
        printf("%s\n", recv_buf);
    }

    close(sock_cli);
    return 0;
}