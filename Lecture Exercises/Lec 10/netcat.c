#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>    /* Internet domain header */
#include <arpa/inet.h>

#define MAXSIZE 128

// build: gcc -o netcat
// use: ./netcat [ip] [port]

int main(int argc, char **argv) {
    char buf[MAXSIZE];

    // Create socket
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("socket");
        exit(1);
    }

    // set up the sockaddr_in struct for connecting
    struct sockaddr_in peer;
    peer.sin_family = AF_INET;
    peer.sin_port = htons(strtol(argv[2], NULL, 10));
    if (inet_pton(AF_INET, argv[1], &peer.sin_addr) < 1) {
        perror("client: inet_pton");
        close(sock_fd);
        exit(1);
    }

    // connect the socket
    if (connect(sock_fd, (struct sockaddr *)&peer, sizeof(peer)) < -1) 
    {
        perror("connect");
        close(sock_fd);
        exit(1);
    }

    while(1) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        FD_SET(sock_fd, &fds);

        int returnval = select(sock_fd + 1, &fds, NULL, NULL, NULL);

        if (returnval < 0) {
            perror("select");
            close(sock_fd);
            exit(1);
        }

        if (FD_ISSET(STDIN_FILENO, &fds)) {
            int k = read(STDIN_FILENO, buf, sizeof(buf));
            if (k > 0) {
                buf[k-1] = '\r';
                buf[k] = '\n';

                write(sock_fd, buf, k + 1);
            }
        }

        if (FD_ISSET(sock_fd, &fds)) {
            int k = read(STDIN_FILENO, buf, sizeof(buf));
            if (k > 0) {
                write(STDOUT_FILENO, buf, k);
            }
        }

    }

    close(sock_fd);

    return 0;
}
