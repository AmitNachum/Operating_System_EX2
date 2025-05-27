#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define BUF_SIZE 1024
#define UDS_STREAM "/tmp/uds_stream.sock"
#define UDS_DGRAM "/tmp/uds_dgram.sock"

int main(int argc, char *argv[]) {
    char *uds_path = NULL;
    //int tcp_flag = 0, udp_flag = 0;

    int opt;
    while ((opt = getopt(argc, argv, "f:T:U:")) != -1) {
        switch (opt) {
            case 'f':
                uds_path = optarg;
                break;
            case 'T':
            case 'U':
                fprintf(stderr, "Error: This client only supports UNIX domain sockets. Do not use -T or -U.\n");
                exit(EXIT_FAILURE);
        }
    }

    if (!uds_path) {
        fprintf(stderr, "Usage: %s -f <UNIX socket path>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char buf[BUF_SIZE];
    int sock_fd = -1;

    if (strcmp(uds_path, UDS_STREAM) == 0) {
        struct sockaddr_un addr;
        sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (sock_fd < 0) { perror("socket stream"); exit(1); }

        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, UDS_STREAM);

        if (connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            perror("connect"); exit(1);
        }

        printf("Connected to molecular_suppliers (UDS-STREAM).\n");
        while (1) {
            printf("> ");
            if (fgets(buf, sizeof(buf), stdin) == NULL) break;
            if (strncmp(buf, "exit", 4) == 0) break;

                if (strncmp(buf, "ADD", 3) == 0 && strstr(buf, " ") == NULL) {
                printf("Usage: ADD <atom> <amount>\n");
                continue;
                }


            if (strncmp(buf, "ADD", 3) == 0) {
                send(sock_fd, buf, strlen(buf), 0);
                char response[BUF_SIZE];
                printf("[CLIENT] Sent: '%s'\n", buf);
                int n = recv(sock_fd, response, sizeof(response) - 1, 0);
                printf("[CLIENT] Received %d bytes\n", n);

                if (n > 0) {
                    response[n] = '\0';
                    printf("Server: %s", response);
                }
            } else {
                printf("ERROR: Only ADD command supported on uds_stream.sock\n");
            }
        }

        close(sock_fd);
        return 0;

    } else if (strcmp(uds_path, UDS_DGRAM) == 0) {
        struct sockaddr_un server_addr, client_addr;
        sock_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (sock_fd < 0) { perror("socket dgram"); exit(1); }

        memset(&client_addr, 0, sizeof(client_addr));
        client_addr.sun_family = AF_UNIX;
        strcpy(client_addr.sun_path, "/tmp/cli_uds_dgram.sock");
        unlink(client_addr.sun_path);
        if (bind(sock_fd, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0) {
            perror("bind"); exit(1);
        }

        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sun_family = AF_UNIX;
        strcpy(server_addr.sun_path, UDS_DGRAM);

        printf("Connected to molecular_suppliers (UDS-DGRAM).\n");
        while (1) {
            printf("> ");
            if (fgets(buf, sizeof(buf), stdin) == NULL) break;
            if (strncmp(buf, "exit", 4) == 0) break;

            if (strncmp(buf, "DELIVER", 7) == 0) {
                sendto(sock_fd, buf, strlen(buf), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
                char response[BUF_SIZE];
                int n = recvfrom(sock_fd, response, sizeof(response) - 1, 0, NULL, NULL);
                if (n > 0) {
                    response[n] = '\0';
                    printf("Server: %s", response);
                }
            } else {
                printf("ERROR: Only DELIVER command supported on uds_dgram.sock\n");
            }
        }

        close(sock_fd);
        unlink("/tmp/cli_uds_dgram.sock");
        return 0;

    } else {
        fprintf(stderr, "Error: Unknown UDS path '%s'\n", uds_path);
        exit(EXIT_FAILURE);
    }

    return 0;
}
