#include "../Q_3/drinks.h"
#include <getopt.h>
#include <unistd.h>
#include <signal.h>
#include "../Q_2/formulas.h"
#include "../Q_1/atoms.h"
#define BUF_SIZE 1024
extern void __gcov_dump (void);

void handle_timout(){

    printf("Timeout reached.\nShutting down...");
    exit(0);
}


int main(int argc, char *argv[]) {
     int tcp_port = -1;
     int udp_port = -1;
     int timeout = -1;
     water = 100;
     alcohol = 100;
     glucose = 100;
     carbon_dioxide = 100;

    struct option long_options[] = {
        {"tcp-port",   required_argument, 0, 'T'},
        {"udp-port",   required_argument, 0, 'U'},
        {"oxygen",     required_argument, 0, 'o'},
        {"carbon",     required_argument, 0, 'c'},
        {"hydrogen",   required_argument, 0, 'h'},
        {"timeout",    required_argument, 0, 't'},
        {0, 0, 0, 0}
    };



int opt;

    while ((opt = getopt_long(argc, argv, "T:U:o:c:h:t:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'T':
                if (optarg != NULL)
                    tcp_port = atoi(optarg);
                else {
                    fprintf(stderr, "Missing argument for -T (TCP port)\n");
                    exit(1);
                }
                break;

                case 'U':
                    if (optarg != NULL)
                    udp_port = atoi(optarg);
                else {
                    fprintf(stderr, "Missing argument for -U (UDP port)\n");
                    exit(1);
                }
                break;

                case 'o':
                if (optarg != NULL)
                    oxygen = atoi(optarg);
                break;

                case 'c':
                if (optarg != NULL)
                    carbon = atoi(optarg);
                break;

                case 'h':
                if (optarg != NULL)
                    hydrogen = atoi(optarg);
                break;

                case 't':
                if (optarg != NULL)
                    timeout = atoi(optarg);
                break;

            default:
                fprintf(stderr, "Usage: %s -T <tcp-port> -U <udp-port> [options]\n", argv[0]);
                exit(1);
        }
    }


     if(tcp_port == -1 || udp_port == -1){
        fprintf(stderr,"Error: -T <tcp_port> and -U <udp_port> are mandatory\n");
        exit(EXIT_FAILURE);
     }

     printf("TCP Port: %d\nUDP Port: %d\nOxygen: %d\nCarbon: %d\nHydrogen: %d\nTimeout: %d\n",
           tcp_port, udp_port, oxygen, carbon, hydrogen, timeout); // debug for checking

    if(timeout != - 1){
    signal(SIGALRM,handle_timout);
    alarm(timeout);
    }else {
    printf("No timeout specified. Server will run indefinitely.\n");
    }

    int listener_fd, udp_fd, tcp_fd;
    struct sockaddr_in tcp_addr, client_addr, udp_addr;
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

    tcp_addr.sin_family = AF_INET;
    tcp_addr.sin_addr.s_addr = INADDR_ANY;
    tcp_addr.sin_port = htons(tcp_port);
    memset(&(tcp_addr.sin_zero), '\0', 8);

    bind(listener_fd, (struct sockaddr *)&tcp_addr, sizeof(tcp_addr));
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
        select(fdmax + 1, &read_fds, NULL, NULL, NULL);


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
                tcp_fd =accept(listener_fd, (struct sockaddr *)&client_addr, &addrlen);
                if (tcp_fd != -1) {
                    FD_SET(tcp_fd, &master_set);
                    if (tcp_fd > fdmax) fdmax = tcp_fd;
                    printf("New TCP connection on socket %d\n", tcp_fd);
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
