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
    int sock_cli;
    char send_buf[] = "send udp msg";
    unsigned int size;
    ssize_t len;

    sockaddr_in addr_svr;
    memset(&addr_svr, 0, sizeof(addr_svr));
    addr_svr.sin_family = AF_INET;
    addr_svr.sin_addr.s_addr = INADDR_ANY;
    addr_svr.sin_port = htons(9000);

    sock_cli = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock_cli < 0)
    {
        perror("create socker err");
        return 0;
    }

    size = sizeof(addr_svr);
    len = sendto(sock_cli, send_buf, strlen(send_buf), 0, (sockaddr *)&addr_svr, sizeof(struct sockaddr));
    if (len < 0)
    {
        perror("sendto date err");
        return 0;
    }

    printf("send successful\n");

    close(sock_cli);
    return 0;
}