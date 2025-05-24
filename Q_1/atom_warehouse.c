#include "atoms.h"
#define MAX_CLIENT FD_SETSIZE
#define BUF_SIZE 1024

unsigned int hydrogen = 0;
unsigned int oxygen = 0;
unsigned int carbon = 0;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = atoi(argv[1]);
    int listener_fd;
    int tcp_fd;
    struct sockaddr_in tcp_addr;
    struct sockaddr_in client_addr;
    socklen_t addrlen;

    char buf[BUF_SIZE];
    int nbytes;

    fd_set master_set, read_fds;
    int fdmax;

    FD_ZERO(&master_set);
    FD_ZERO(&read_fds);

    if ((listener_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int yes = 1;
    if (setsockopt(listener_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    tcp_addr.sin_family = AF_INET;
    tcp_addr.sin_addr.s_addr = INADDR_ANY;
    tcp_addr.sin_port = htons(port);
    memset(&(tcp_addr.sin_zero), '\0', 8);

    if (bind(listener_fd, (struct sockaddr *)&tcp_addr, sizeof(tcp_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(listener_fd, 10) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    FD_SET(listener_fd, &master_set);
    fdmax = listener_fd;

    printf("Atom warehouse server listening on port %d...\n", port);
    
    while (true) {
        read_fds = master_set;
        int activity = select(fdmax + 1, &read_fds, NULL, NULL, NULL);

        if(activity == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == listener_fd) {
                    addrlen = sizeof(client_addr);
                    if ((tcp_fd = accept(listener_fd, (struct sockaddr *)&client_addr, &addrlen)) == -1) {
                        perror("accept");
                    } else {
                        FD_SET(tcp_fd, &master_set);
                        if (tcp_fd > fdmax) fdmax = tcp_fd;
                        printf("New connection on socket %d\n", tcp_fd);
                    }
                } else {
                    if ((nbytes = recv(i, buf, sizeof(buf) - 1, 0)) <= 0) {
                        if (nbytes == 0) {
                            printf("Socket %d hung up\n", i);
                        } else {
                            perror("recv");
                        }
                        close(i);
                        FD_CLR(i, &master_set);
                    } else {
                        buf[nbytes] = '\0';

                        if(buf[0] == '\n' || buf[0] == '\0') continue;

                        char command[BUF_SIZE];
                        char atom[BUF_SIZE];
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
                                printf("Error: unknown atom type '%s'\n", atom);
                                continue;
                            }
                            char response[BUF_SIZE];

                            snprintf(response,sizeof(response),
                            "Hydrogen: %u\nOxygen: %u\nCarbon: %u\n",
                            hydrogen,oxygen,carbon);

                            send(i,response,strlen(response),0);

                            printf("%s",response);
                        } else {
                            printf("Error: invalid command '%s'\n", buf);
                            char *err = "Error: Invalid command format\n";
                            send(i, err, strlen(err), 0);
 
                        }
                    }
                }
            }
        }
    }

    return 0;
}
