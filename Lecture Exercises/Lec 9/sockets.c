#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 50000

int main(int argc, char **argv) {
    char *msg = "A Grande Non-Fat No Whip Iced Peppermint White Chocolate Mocha, please!\r\n";

    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd == -1) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server.sin_addr) < 0) {
        perror("client: inet_pton");
        close(sock_fd);
        exit(1);
    }

    if (connect(sock_fd, (struct sockaddr *)&server, sizeof(server)) == -1) {
        perror("connect");
        close(sock_fd);
        exit(1);
    }

    printf("Client connected\n");

    if (write(sock_fd, msg, strlen(msg)) == -1) {
        perror("write to server");
        close(sock_fd);
        exit(1);
    }

    char buf[128];
    int num_read = read(sock_fd, buf, sizeof(buf) - 1);

    if (num_read == -1) {
        perror("read from server");
        close(sock_fd);
        exit(1);
    }

    buf[num_read] = '\0';

    printf("Server wrote: %s", buf);

    close(sock_fd);

    return 0;
}