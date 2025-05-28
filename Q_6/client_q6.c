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
#define UDP_PORT "6666"
#define BUF_SIZE 1024

int main() {
    int sock_fd;
    int sock_udp_fd;
    struct addrinfo hints, *res;
    struct addrinfo hints_udp, *res_udp;


    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if(getaddrinfo(SERVER,PORT,&hints,&res) != 0){
        perror("getaddrinfo");
        exit(EXIT_FAILURE);
    }

    memset(&hints_udp,0,sizeof(hints_udp));
    hints_udp.ai_family = AF_INET;
    hints_udp.ai_socktype = SOCK_DGRAM;

    if(getaddrinfo(SERVER,UDP_PORT,&hints_udp,&res_udp) != 0){
        perror("getaddrinfo");
         exit(EXIT_FAILURE);
    }

    sock_udp_fd = socket(res_udp->ai_family,res_udp->ai_socktype,res_udp->ai_protocol);
    if(sock_udp_fd < 0){
        perror("socket");
        exit(EXIT_FAILURE);
    }

    sock_fd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
    if(sock_fd < 0){
        perror("socket");
        exit(EXIT_FAILURE);
    }


    




    char buf[BUF_SIZE];
    printf("Connected to molecular_suppliers.\nType your commands: \n ");

    while(1){
        printf("> ");

        if(fgets(buf,sizeof(buf),stdin) == NULL) break;

        if(strncmp(buf,"exit",4) == 0) break;

        if(strncmp(buf,"ADD",3) == 0){
        connect(sock_fd,res->ai_addr, res->ai_addrlen);
        send(sock_fd,buf,strlen(buf),0);

        char response[BUF_SIZE];
        int n = recv(sock_fd,response,sizeof(response) - 1,0);
        if(n > 0){
            response[n] = '\0';
            printf("Server: %s ",response);
        }
    }
        else if(strncmp(buf,"DELIVER",7) == 0){
            
            sendto(sock_udp_fd,
                buf,strlen(buf),
            0,res_udp->ai_addr
            ,res_udp->ai_addrlen);
           

            char response[BUF_SIZE];
            struct sockaddr_storage from;

            socklen_t from_len = sizeof(from);
            int n = recvfrom(sock_udp_fd,response,sizeof(response) - 1,
             0, (struct sockaddr *)&from,&from_len);


             if(n > 0){
                response[n] ='\0';
                printf("Server: %s",response);
             }
             else{
                printf("Unknown command. Use ADD or DELIVER.\n");
             }
        }
    }   

    freeaddrinfo(res);
    close(sock_fd);
    close(sock_udp_fd);
    freeaddrinfo(res_udp);
    printf("\nDisconnected\n");
return 0;

}