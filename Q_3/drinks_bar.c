#include "drinks.h"

#define BUF_SIZE 1024

unsigned int water = 0;
unsigned int carbon_dioxide = 0;
unsigned int glucose = 0;
unsigned int alcohol = 0;

unsigned int hydrogen = 0;
unsigned int oxygen = 0;
unsigned int carbon = 0;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <tcp_port> <udp_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int tcp_port = atoi(argv[1]);
    int udp_port = atoi(argv[2]);

    int listener_fd, udp_fd, new_fd;
    struct sockaddr_in server_addr, client_addr, udp_addr;
    socklen_t addrlen;
    char buf[BUF_SIZE];
    int nbytes;

    fd_set master_set, read_fds;
    int fdmax;

    FD_ZERO(&master_set);
    FD_ZERO(&read_fds);

    listener_fd = socket(AF_INET, SOCK_STREAM, 0);
    int yes = 1;
    setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(tcp_port);
    memset(&(server_addr.sin_zero), '\0', 8);

    bind(listener_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(listener_fd, 10);

    FD_SET(listener_fd, &master_set);
    fdmax = listener_fd;

    udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    udp_addr.sin_family = AF_INET;
    udp_addr.sin_addr.s_addr = INADDR_ANY;
    udp_addr.sin_port = htons(udp_port);
    memset(&(udp_addr.sin_zero), '\0', 8);

    bind(udp_fd, (struct sockaddr *)&udp_addr, sizeof(udp_addr));

    FD_SET(udp_fd, &master_set);
    if (udp_fd > fdmax) fdmax = udp_fd;

    FD_SET(STDIN_FILENO, &master_set);
    if (STDIN_FILENO > fdmax) fdmax = STDIN_FILENO;

    printf("Atom warehouse server running on TCP %d and UDP %d...\n", tcp_port, udp_port);

    while (1) {
        read_fds = master_set;
        struct timeval timeout = {15, 0};
        int activity = select(fdmax + 1, &read_fds, NULL, NULL, &timeout);

        if (activity <= 0) {
            if (activity == 0) printf("Timeout: No activity.\n");
            continue;
        }

        for (int i = 0; i <= fdmax; i++) {
            if (!FD_ISSET(i, &read_fds)) continue;

            if (i == STDIN_FILENO) {
                if (!fgets(buf, sizeof(buf), stdin)) continue;
                buf[strcspn(buf, "\n")] = 0;

                char *cmd = strtok(buf, " ");
                char *drink = strtok(NULL, "");

                if (!cmd || !drink || strcmp(cmd, "GEN") != 0) {
                    printf("Usage: GEN <drink name>\n");
                    continue;
                }

                while (*drink == ' ') drink++;

                if (strcmp(drink, "SOFT DRINK") == 0) {
                    printf("Can generate %d SOFT DRINKS\n", count_soft_drink());
                } else if (strcmp(drink, "VODKA") == 0) {
                    printf("Can generate %d VODKAS\n", count_vodka());
                } else if (strcmp(drink, "CHAMPAGNE") == 0) {
                    printf("Can generate %d CHAMPAGNES\n", count_champagne());
                } else {
                    printf("Invalid drink: '%s'\n", drink);
                }
                continue;
            }

            if (i == udp_fd) {
                struct sockaddr_in udp_client;
                socklen_t addr_len = sizeof(udp_client);
                int n = recvfrom(udp_fd, buf, sizeof(buf) - 1, 0, (struct sockaddr *)&udp_client, &addr_len);
                if (n <= 0) continue;
                buf[n] = '\0';

                char command[BUF_SIZE], molecule[64];
                unsigned int amount;
                char response[BUF_SIZE];

                int fields = sscanf(buf, "%s %s %u", command, molecule, &amount);
                if (fields != 3 || strcmp(command, "DELIVER") != 0) {
                    snprintf(response, sizeof(response), "Error: Invalid command format\n");
                    sendto(udp_fd, response, strlen(response), 0, (struct sockaddr *)&udp_client, addr_len);
                    printf("UDP %s", response);
                    continue;
                }

                int success = 0;
                if (strcmp(molecule, "WATER") == 0) success = create_h2o(amount), water += success ? amount : 0;
                else if (strcmp(molecule, "CARBON_DIOXIDE") == 0) success = create_carbon_dioxide(amount), carbon_dioxide += success ? amount : 0;
                else if (strcmp(molecule, "ALCOHOL") == 0) success = create_alcohol(amount), alcohol += success ? amount : 0;
                else if (strcmp(molecule, "GLUCOSE") == 0) success = create_glucose(amount), glucose += success ? amount : 0;
                else {
                    snprintf(response, sizeof(response), "Error: Unknown molecule '%s'\n", molecule);
                    sendto(udp_fd, response, strlen(response), 0, (struct sockaddr *)&udp_client, addr_len);
                    printf("UDP %s", response);
                    continue;
                }

                snprintf(response, sizeof(response), "%s: %s x%u\n",
                         success ? "DELIVERED" : "FAILED: Not enough atoms for", molecule, amount);
                sendto(udp_fd, response, strlen(response), 0, (struct sockaddr *)&udp_client, addr_len);
                printf("UDP %s", response);
                continue;
            }

            if (i == listener_fd) {
                addrlen = sizeof(client_addr);
                new_fd = accept(listener_fd, (struct sockaddr *)&client_addr, &addrlen);
                if (new_fd != -1) {
                    FD_SET(new_fd, &master_set);
                    if (new_fd > fdmax) fdmax = new_fd;
                    printf("New TCP connection on socket %d\n", new_fd);
                }
                continue;
            }

            if ((nbytes = recv(i, buf, sizeof(buf) - 1, 0)) <= 0) {
                if (nbytes == 0) printf("TCP socket %d disconnected\n", i);
                else perror("recv");
                close(i);
                FD_CLR(i, &master_set);
                continue;
            }

            buf[nbytes] = '\0';
            if (buf[0] == '\n' || buf[0] == '\0') continue;

            char command[BUF_SIZE], atom[BUF_SIZE];
            unsigned int amount;

            if (sscanf(buf, "%s %s %u", command, atom, &amount) == 3 && strcmp(command, "ADD") == 0) {
                if (strcmp(atom, "HYDROGEN") == 0) hydrogen += amount;
                else if (strcmp(atom, "OXYGEN") == 0) oxygen += amount;
                else if (strcmp(atom, "CARBON") == 0) carbon += amount;
                else {
                    char *err = "Error: Unknown atom type\n";
                    send(i, err, strlen(err), 0);
                    continue;
                }

                char response[BUF_SIZE];
                snprintf(response, sizeof(response),
                         "Hydrogen: %u\nOxygen: %u\nCarbon: %u\n", hydrogen, oxygen, carbon);
                send(i, response, strlen(response), 0);
                printf("TCP %s", response);
            } else {
                char *err = "Error: Invalid command format\n";
                send(i, err, strlen(err), 0);
            }
        }
    }

    return 0;
}
