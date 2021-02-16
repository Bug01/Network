#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SVR_PORT 9999
#define MAX_BUFLEN 1024

static void udpcli_echo(int sockfd, sockaddr *addr_svr)
{
    char buf[MAX_BUFLEN] = "UDP TEST";
    sockaddr_in from;
    socklen_t len = sizeof(from);

    sendto(sockfd, buf, MAX_BUFLEN, 0, addr_svr, sizeof(*addr_svr));

    recvfrom(sockfd, buf, MAX_BUFLEN, 0, (sockaddr *)&from, &len);

    printf("recv svr data: %s.\n", buf);
}

int main(int argc, char *argv[])
{
    int sockfd;
    sockaddr_in addr_svr;
    memset(&addr_svr, 0, sizeof(addr_svr));
    addr_svr.sin_family = AF_INET;
    addr_svr.sin_addr.s_addr = INADDR_ANY;
    addr_svr.sin_port = htons(SVR_PORT);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        printf("create socket err.\n");
        return -1;
    }

    udpcli_echo(sockfd, (sockaddr *)&addr_svr);

    close(sockfd);
    return 0;
}