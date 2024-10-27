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

// gcc -Ofast -std=c99 -Wall -Wextra -pedantic -o server server.c

int main(void)
{
    // make a socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket error:");
        return EXIT_FAILURE;
    }

    // allow port reuse
    int opt_val = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const void*)&opt_val, sizeof(opt_val)) == -1) {
        perror("setsockopt error:");
        goto serv_cleanup;
    }

    // setup addr structure
    struct sockaddr_in serv_addr = { 0 };
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    struct in_addr ip_addr = { 0 };
    if (inet_aton("0.0.0.0", &ip_addr) == 0) {
        fprintf(stderr, "Invalid IP address was specified\n");
        goto serv_cleanup;
    }

    serv_addr.sin_addr = ip_addr;

    // bind to a port
    if (bind(sock, (const struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1) {
        perror("binding error:");
        goto serv_cleanup;
    }

    if (listen(sock, 5) == -1) {
        perror("listening error:");
        goto serv_cleanup;
    }

    printf("+ server listening on port %d...\n", PORT);

    // accept on socket
    int client_sock, size;
    const char* msg;
    unsigned char buf[BUFSIZE] = { 0 };
    for (;;) {
        if ((client_sock = accept(sock, NULL, NULL)) == -1) {
            perror("accepting error:");
            goto serv_client_cleanup;
        }

        // deal with socket

        // read from client
        if (read(client_sock, buf, BUFSIZE) == -1) {
            perror("reading error:");
            goto serv_client_cleanup;
        }

        printf("msg from client: %s\n", (const char*)buf);

        // server kill switch
        if (strncmp((const char*)buf, "##", 2) == 0) {
            printf("+ hit server kill switch\n");

            msg = "bye bye now";
            size = strlen(msg);
            if (write(client_sock, msg, size + 1) == -1) {
                perror("write error:");
                goto serv_client_cleanup;
            }

            break;
        }

        // send back to client
        msg = "kili kitya eeyo!";
        size = strlen(msg);
        if (write(client_sock, msg, size + 1) == -1) {
            perror("write error:");
            goto serv_client_cleanup;
        }

        printf("msg sent: %s\n", msg);
    }

    // close up shop
    printf("+ clean up...\n");
    if (close(client_sock) == -1) {
        perror("closing client sock:");
        return EXIT_FAILURE;
    }
    if (close(sock) == -1) {
        perror("closing server sock:");
        return EXIT_FAILURE;
    }

    return 0;

serv_client_cleanup:
    printf("+ clean up...\n");
    if (close(client_sock) == -1) {
        perror("closing client sock:");
        return EXIT_FAILURE;
    }
    if (close(sock) == -1) {
        perror("closing server sock:");
        return EXIT_FAILURE;
    }
    return EXIT_FAILURE;

serv_cleanup:
    printf("+ clean up...\n");
    if (close(sock) == -1) {
        perror("closing server sock:");
        return EXIT_FAILURE;
    }
    return EXIT_FAILURE;
}
