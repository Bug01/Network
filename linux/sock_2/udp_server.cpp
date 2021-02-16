#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SVR_PORT 9999
#define MAX_BUFLEN 1024

static void udpsvr_echo(int sockfd, sockaddr *addr_cli)
{
    char buf[MAX_BUFLEN];
    char send[MAX_BUFLEN] = {"recv udp msg"};

    ssize_t recvSize;
    socklen_t addr_len;

    while (true)
    {
        addr_len = sizeof(*addr_cli);
        recvSize = recvfrom(sockfd, buf, MAX_BUFLEN, 0, addr_cli, &addr_len);
        printf("recv cli msg %s.\n", buf);

        sendto(sockfd, send, MAX_BUFLEN, 0, addr_cli, addr_len);
    }
}

int main(int argc, char *argv[])
{
    int sockfd;
    sockaddr_in addr_svr, addr_cli;

    memset(&addr_svr, 0, sizeof(addr_svr));
    addr_svr.sin_family = AF_INET;
    addr_svr.sin_addr.s_addr = INADDR_ANY;
    addr_svr.sin_port = htons(SVR_PORT);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        printf("socket init error!\n");
        return -1;
    }

    bind(sockfd, (sockaddr *)&addr_svr, sizeof(addr_svr));

    udpsvr_echo(sockfd, (sockaddr *)&addr_cli);

    close(sockfd);
    return 0;
}