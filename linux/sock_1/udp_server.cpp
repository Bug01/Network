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
    int sock_svr;
    struct sockaddr_in addr_svr, addr_cli;

    ssize_t len;
    unsigned int size;
    char data_buf[MAX_BUF_LENG];

    memset(&addr_svr, 0, sizeof(addr_svr));
    addr_svr.sin_family = AF_INET;
    addr_svr.sin_addr.s_addr = INADDR_ANY;
    addr_svr.sin_port = htons(9000);

    sock_svr = socket(PF_INET, SOCK_DGRAM, 0);
    if (sock_svr < 0)
    {
        perror("create socket error.");
        return 0;
    }

    if (bind(sock_svr, (sockaddr *)&addr_svr, sizeof(addr_svr)) < 0)
    {
        perror("bind socket error.");
        return 0;
    }

    size = sizeof(addr_cli);
    len = recvfrom(sock_svr, data_buf, MAX_BUF_LENG, 0, (sockaddr *)&addr_svr, &size);
    if (len < 0)
    {
        perror("recvfrom data error.");
        return 0;
    }

    data_buf[len] = '\0';
    printf("recv data: %s\n", data_buf);

    close(sock_svr);
    return 0;
}