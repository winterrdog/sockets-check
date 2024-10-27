#define _DEFAULT_SOURCE

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 3003
#define BUFSIZE 256

// gcc -std=c99 -Wall -Wextra -pedantic -o client client.c

int main(void)
{
    // make a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket error:");
        return EXIT_FAILURE;
    }

    // setup addr structure
    struct sockaddr_in serv_addr = { 0 };
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    struct in_addr ip_addr = { 0 };
    if (inet_aton("127.0.0.1", &ip_addr) == 0) {
        fprintf(stderr, "Invalid IP address was specified\n");
        goto cleanup;
    }

    serv_addr.sin_addr = ip_addr;

    if (connect(sock, (const struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("connect error:");
        goto cleanup;
    }

    // write
    const char* msg = "mwana kitokota!!";
    int size = strlen(msg);
    if (write(sock, (const void*)msg, size + 1) == -1) {
        perror("writing error");
        goto cleanup;
    }

    // read
    unsigned char buf[BUFSIZE];
    if (read(sock, (void*)buf, BUFSIZE) == -1) {
        perror("reading error:");
        goto cleanup;
    }

    printf("server said: %s\n", (const char*)buf);

    printf("+ clean up...\n");
    if (close(sock) == -1) {
        perror("closing client sock:");
        return EXIT_FAILURE;
    }

    return 0;

cleanup:
    printf("+ clean up...\n");
    if (close(sock) == -1) {
        perror("closing client sock:");
        return EXIT_FAILURE;
    }
    return EXIT_FAILURE;
}
