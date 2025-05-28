
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/select.h>
#include "formulas.h"
#include <gcov.h>
#include <unistd.h>

extern void __gcov_dump(void);

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    hydrogen = 100;
    carbon = 100;
    oxygen = 100;
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <tcp_port> <udp_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int tcp_port = atoi(argv[1]);
    int udp_port = atoi(argv[2]);

    int listener_fd, udp_fd, tcp_fd;
    struct sockaddr_in tcp_addr, client_addr, udp_addr;
    socklen_t addrlen;
    char buf[BUF_SIZE];
    int nbytes;

    fd_set master_set, read_fds;
    int fdmax;

    FD_ZERO(&master_set);
    FD_ZERO(&read_fds);

    if ((listener_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("TCP socket");
        exit(EXIT_FAILURE);
    }

    int yes = 1;
    setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    tcp_addr.sin_family = AF_INET;
    tcp_addr.sin_addr.s_addr = INADDR_ANY;
    tcp_addr.sin_port = htons(tcp_port);
    memset(&(tcp_addr.sin_zero), '\0', 8);

    if (bind(listener_fd, (struct sockaddr *)&tcp_addr, sizeof(tcp_addr)) == -1) {
        perror("TCP bind");
        exit(EXIT_FAILURE);
    }

    if (listen(listener_fd, 10) == -1) {
        perror("TCP listen");
        exit(EXIT_FAILURE);
    }

    FD_SET(listener_fd, &master_set);
    fdmax = listener_fd;

    if ((udp_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("UDP socket");
        exit(EXIT_FAILURE);
    }

    udp_addr.sin_family = AF_INET;
    udp_addr.sin_addr.s_addr = INADDR_ANY;
    udp_addr.sin_port = htons(udp_port);
    memset(&(udp_addr.sin_zero), '\0', 8);

    if (bind(udp_fd, (struct sockaddr *)&udp_addr, sizeof(udp_addr)) < 0) {
        perror("UDP bind");
        exit(EXIT_FAILURE);
    }

    FD_SET(udp_fd, &master_set);
    if (udp_fd > fdmax) fdmax = udp_fd;

    printf("Atom warehouse server running on TCP %d and UDP %d...\n", tcp_port, udp_port);

    while (true) {
        read_fds = master_set;

        int activity = select(fdmax + 1, &read_fds, NULL, NULL, NULL);

        if (activity == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i <= fdmax; i++) {
            if (!FD_ISSET(i, &read_fds)) continue;

            if (i == udp_fd) {
                struct sockaddr_in udp_client;
                socklen_t addr_len = sizeof(udp_client);
                int n = recvfrom(udp_fd, buf, sizeof(buf) - 1, 0, (struct sockaddr *)&udp_client, &addr_len);
                if (n <= 0) continue;
                buf[n] = '\0';

                char response[BUF_SIZE];
                if (strncmp(buf, "DELIVER ", 8) == 0) {
                    char molecule[64];
                    unsigned int amount;
                    int parsed = sscanf(buf + 8, "%63s %u", molecule, &amount);
                    if (parsed != 2) {
                        snprintf(response, sizeof(response), "Error: Invalid DELIVER format\n");
                        sendto(udp_fd, response, strlen(response), 0, (struct sockaddr *)&udp_client, addr_len);
                        
                        continue;
                    }

                    int success = 0;
                    if (strcmp(molecule, "WATER") == 0) {
                        success = create_h2o(amount);
                        if (success) water += amount;
                    } else if (strcmp(molecule, "CARBON_DIOXIDE") == 0) {
                        success = create_carbon_dioxide(amount);
                        if (success) carbon_dioxide += amount;
                    } else if (strcmp(molecule, "ALCOHOL") == 0) {
                        success = create_alcohol(amount);
                        if (success) alcohol += amount;
                    } else if (strcmp(molecule, "GLUCOSE") == 0) {
                        success = create_glucose(amount);
                        if (success) glucose += amount;
                    } else {
                        snprintf(response, sizeof(response), "Error: Unknown molecule '%s'\n", molecule);
                        sendto(udp_fd, response, strlen(response), 0, (struct sockaddr *)&udp_client, addr_len);
                        continue;
                    }
                    

                    snprintf(response, sizeof(response), "%s: %s x%u\n",
                             success ? "DELIVERED" : "FAILED: Not enough atoms for",
                             molecule, amount);
                    sendto(udp_fd, response, strlen(response), 0, (struct sockaddr *)&udp_client, addr_len);
                    printf("UDP %s", response);
                } else {
                    snprintf(response, sizeof(response), "Error: Unknown command\n");
                    sendto(udp_fd, response, strlen(response), 0, (struct sockaddr *)&udp_client, addr_len);
                }
                  if (getenv("COVERAGE_MODE")) {
                    usleep(1000);
                        fflush(NULL);
                        __gcov_dump(); 
                        exit(0);
                }
            } else if (i == listener_fd) {
                addrlen = sizeof(client_addr);

                if ((tcp_fd = accept(listener_fd, (struct sockaddr *)&client_addr, &addrlen)) == -1) {
                    perror("accept");
                } else {
                    FD_SET(tcp_fd, &master_set);
                    if (tcp_fd > fdmax) fdmax = tcp_fd;
                    printf("New TCP connection on socket %d\n", tcp_fd);
                }
            } else {
                if ((nbytes = recv(i, buf, sizeof(buf) - 1, 0)) <= 0) {
                    if (nbytes == 0) {
                        printf("TCP socket %d disconnected\n", i);
                    } else {
                        perror("recv");
                    }
                    close(i);
                    FD_CLR(i, &master_set);
                } else {
                    buf[nbytes] = '\0';
                    if (buf[0] == '\n' || buf[0] == '\0') continue;

                    char command[BUF_SIZE], atom[BUF_SIZE];
                    unsigned int amount;

                    if (sscanf(buf, "%s %s %u", command, atom, &amount) == 3 &&
                        strcmp(command, "ADD") == 0) {

                        if (strcmp(atom, "HYDROGEN") == 0) {
                            hydrogen += amount;
                        } else if (strcmp(atom, "OXYGEN") == 0) {
                            oxygen += amount;
                        } else if (strcmp(atom, "CARBON") == 0) {
                            carbon += amount;
                        } else {
                            char *err = "Error: Unknown atom type\n";
                            send(i, err, strlen(err), 0);
                            continue;
                        }

                        char response[BUF_SIZE];
                        snprintf(response, sizeof(response),
                                 "Hydrogen: %u\nOxygen: %u\nCarbon: %u\n",
                                 hydrogen, oxygen, carbon);
                        send(i, response, strlen(response), 0);
                        printf("TCP %s", response);

                    } else {
                        char *err = "Error: Invalid command format\n";
                        send(i, err, strlen(err), 0);
                    }
                }
            }
        }
    }

    return 0;
}
