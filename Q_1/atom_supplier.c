#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT "5555"
#define SERVER "127.0.0.1"
# define BUF_SIZE 1024

int main() {
    int sock_fd;
    struct addrinfo hints, *res;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if(getaddrinfo(SERVER,PORT,&hints,&res) != 0){
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }

    sock_fd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
    if(sock_fd < 0){
        perror("socket");
        exit(EXIT_FAILURE);
    }

    connect(sock_fd,res->ai_addr, res->ai_addrlen);


    freeaddrinfo(res);


    char buf[BUF_SIZE];
    printf("Connected to atom_wherehouse.\nType your commands: \n ");

    while(1){
        printf("> ");

        if(fgets(buf,sizeof(buf),stdin) == NULL) break;

        if(strncmp(buf,"exit",4) == 0) break;

        send(sock_fd,buf,sizeof(buf),0);

        char response[BUF_SIZE];
        int n = recv(sock_fd,response,sizeof(response) - 1,0);
        if(n > 0){
            response[n] = '\0';
            printf("Server: %s \n",response);
        }
    }
    close(sock_fd);
    printf("\nDisconnected\n");
return 0;
}
