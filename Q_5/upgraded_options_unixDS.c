#include "../Q_3/drinks.h"
#include <getopt.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include "../Q_2/formulas.h"
#include "../Q_1/atoms.h"
#include <gcov.h>
extern void __gcov_dump(void);

#define BUF_SIZE 1024
#define STREAM_UNIX "/tmp/uds_stream.sock"
#define UDP_UNIX "/tmp/uds_dgram.sock"


void handle_timout() {
    printf("Timeout reached.\nShutting down...\n");
    exit(0);
}

static char* tcp_unix = NULL;
static char* udp_unix = NULL;

int main(int argc, char *argv[]) {

    unlink(STREAM_UNIX);
    unlink(UDP_UNIX);

    int tcp_port = -1, udp_port = -1, timeout = -1;

    struct option long_options[] = {
        {"tcp-port", required_argument, 0, 'T'},
        {"udp-port", required_argument, 0, 'U'},
        {"oxygen", required_argument, 0, 'o'},
        {"carbon", required_argument, 0, 'c'},
        {"hydrogen", required_argument, 0, 'h'},
        {"timeout", required_argument, 0, 't'},
        {"uds_stream", required_argument, 0, 's'},
        {"uds_dgram", required_argument, 0, 'd'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "T:U:o:c:h:t:s:d:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'T': tcp_port = atoi(optarg); break;
            case 'U': udp_port = atoi(optarg); break;
            case 'o': oxygen = atoi(optarg); break;
            case 'c': carbon = atoi(optarg); break;
            case 'h': hydrogen = atoi(optarg); break;
            case 't': timeout = atoi(optarg); break;
            case 's': tcp_unix = optarg; break;
            case 'd': udp_unix = optarg; break;
            default:
                fprintf(stderr, "Usage: %s -T <tcp-port> -U <udp-port> [options]\n", argv[0]);
                exit(1);
        }
    }

printf("TCP Port: %d\nUDP Port: %d\nUDP PATH: %s\nTCP PATH: %s\nOxygen: %d\nCarbon: %d\nHydrogen: %d\nTimeout: %d\n",
       tcp_port != -1 ? tcp_port : 0,
       udp_port != -1 ? udp_port : 0,
       udp_unix ? udp_unix : "(null)",                                                                                                                  
       tcp_unix ? tcp_unix : "(null)",
       oxygen, carbon, hydrogen,                                                                                                                                                        
       timeout != -1 ? timeout : 0);

    if(timeout != - 1){
        signal(SIGALRM,handle_timout);
        alarm(timeout);
    }else {
        printf("No timeout specified. Server will run indefinitely.\n");
    }
    if (tcp_unix) {
        int sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (sock_fd == -1) { perror("socket (UDS)"); exit(EXIT_FAILURE); }

        struct sockaddr_un server_socket;
        memset(&server_socket, 0, sizeof(server_socket));
        server_socket.sun_family = AF_UNIX;
        strncpy(server_socket.sun_path, tcp_unix, sizeof(server_socket.sun_path) - 1);

        unlink(server_socket.sun_path);
        if (bind(sock_fd, (struct sockaddr *)&server_socket, sizeof(server_socket)) == -1) {
            perror("bind (UDS)"); exit(EXIT_FAILURE);
        }

        if (listen(sock_fd, 5) == -1) {
            perror("listen (UDS)"); exit(EXIT_FAILURE);
        }

       while (1) {
        int client_fd = accept(sock_fd, NULL, NULL);
            if (client_fd == -1) {
                perror("accept");
                continue;
            }

        char buf[BUF_SIZE]; // declared once per client session

        while (1) {
            int nbytes = recv(client_fd, buf, sizeof(buf) - 1, 0);
            if (nbytes <= 0) {
                if (nbytes == 0) {
                printf("[SERVER] Client disconnected.\n");
                } else {
                    perror("recv (UDS)");
                }
                break;
            }

            buf[nbytes] = '\0';
            buf[strcspn(buf, "\n")] = 0;

            char *cmd = strtok(buf, " ");
            char *atom = strtok(NULL, " ");
            char *amount_str = strtok(NULL, " ");

            if (cmd && atom && amount_str && strcmp(cmd, "ADD") == 0) {
                unsigned int amount = atoi(amount_str);
                if (strcmp(atom, "HYDROGEN") == 0) hydrogen += amount;
                else if (strcmp(atom, "CARBON") == 0) carbon += amount;
                else if (strcmp(atom, "OXYGEN") == 0) oxygen += amount;
                else {
                    char *err = "Error: Unknown atom type\n";
                    printf("%s", err);
                    send(client_fd, err, strlen(err), 0);
                    continue;
                }

                char response[BUF_SIZE];
                snprintf(response, sizeof(response), "Hydrogen: %u\nCarbon: %u\nOxygen: %u\n",
                     hydrogen, carbon, oxygen);
                send(client_fd, response, strlen(response), 0);
                
            } else {
                char *err = "Error: Invalid command format\n";
                printf("%s", err);
                send(client_fd, err, strlen(err), 0);
            }
                if (getenv("COVERAGE_MODE")) {
                    usleep(1000);
                    fflush(NULL);
                    __gcov_dump(); 
                    exit(0);
                }
        }

        close(client_fd);
        }

        return 0;
    }

    if (udp_unix) {
        if (strcmp(udp_unix, UDP_UNIX) != 0) {
            printf("Invalid uds Path\n");
            exit(EXIT_FAILURE);
        }

        int dgram_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (dgram_fd == -1) {
            perror("socket (UDS-DGRAM)");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_un server_addr;
        memset(&server_addr, 0, sizeof(server_addr));
        server_addr.sun_family = AF_UNIX;
        strncpy(server_addr.sun_path, udp_unix, sizeof(server_addr.sun_path) - 1);

        unlink(server_addr.sun_path);
        if (bind(dgram_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
            perror("bind (UDS-DGRAM)");
            close(dgram_fd);
            exit(EXIT_FAILURE);
        }

        printf("[UDS-DGRAM] Server listening on %s\n", udp_unix);

        while (1) {
            char buf[BUF_SIZE];
            struct sockaddr_un client_addr;
            socklen_t client_len = sizeof(client_addr);

            int nbytes = recvfrom(dgram_fd, buf, sizeof(buf) - 1, 0,
                                  (struct sockaddr *)&client_addr, &client_len);
            if (nbytes <= 0) {
                perror("recvfrom (UDS-DGRAM)");
                continue;
            }

            buf[nbytes] = '\0';
            buf[strcspn(buf, "\n")] = 0;

            char *cmd = strtok(buf, " ");
            char *molecule = strtok(NULL, " ");
            char *amount_str = strtok(NULL, " ");

            char response[BUF_SIZE];

            if (!cmd || !molecule || !amount_str || strcmp(cmd, "DELIVER") != 0) {
                snprintf(response, sizeof(response), "Error: Invalid command format\n");
                sendto(dgram_fd, response, strlen(response), 0,
                       (struct sockaddr *)&client_addr, client_len);
                continue;
            }

            unsigned int amount = atoi(amount_str);
            int success = 0;

            if (strcmp(molecule, "WATER") == 0) success = create_h2o(amount), water += success ? amount : 0;
            else if (strcmp(molecule, "CARBON_DIOXIDE") == 0) success = create_carbon_dioxide(amount), carbon_dioxide += success ? amount : 0;
            else if (strcmp(molecule, "ALCOHOL") == 0) success = create_alcohol(amount), alcohol += success ? amount : 0;
            else if (strcmp(molecule, "GLUCOSE") == 0) success = create_glucose(amount), glucose += success ? amount : 0;
            else {
                snprintf(response, sizeof(response), "Error: Unknown molecule '%s'\n", molecule);
                sendto(dgram_fd, response, strlen(response), 0,
                       (struct sockaddr *)&client_addr, client_len);
                continue;
            }

            snprintf(response, sizeof(response), "%s: %s x%u\n",
                     success ? "DELIVERED" : "FAILED: Not enough atoms for", molecule, amount);
            sendto(dgram_fd, response, strlen(response), 0,
                   (struct sockaddr *)&client_addr, client_len);

        }
                if (getenv("COVERAGE_MODE")) {
                    usleep(1000);
                    fflush(NULL);
                    __gcov_dump(); 
                    exit(0);
                }

        close(dgram_fd);
        return 0;
    }


     if(tcp_port == -1 || udp_port == -1){
        fprintf(stderr,"Error: -T <tcp_port> and -U <udp_port> are mandatory\n");
        exit(EXIT_FAILURE);
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
                if (getenv("COVERAGE_MODE")) {
                    usleep(1000);
                    fflush(NULL);
                    __gcov_dump(); 
                    exit(0);
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
                if (strcmp(molecule, "WATER") == 0) success = create_h2o(amount), 
                water += success ? amount : 0;
                else if (strcmp(molecule, "CARBON_DIOXIDE") == 0) success = create_carbon_dioxide(amount), 
                carbon_dioxide += success ? amount : 0;
                else if (strcmp(molecule, "ALCOHOL") == 0) success = create_alcohol(amount), 
                alcohol += success ? amount : 0;
                else if (strcmp(molecule, "GLUCOSE") == 0) success = create_glucose(amount), 
                glucose += success ? amount : 0;
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
            if (getenv("COVERAGE_MODE")) {
                    usleep(1000);
                    fflush(NULL);
                    __gcov_dump(); 
                    exit(0);
                }
        }
    }

    return 0;
}
