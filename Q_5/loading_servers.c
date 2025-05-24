#include "loading_servers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>

/* you'll need these declared exactly once in your main file: */
extern unsigned int hydrogen, oxygen, carbon;
extern unsigned int water, carbon_dioxide, alcohol, glucose;

void start_unix_stream(const char *path) {
    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("stream socket"); exit(1); }
    unlink(path);
    struct sockaddr_un addr = { .sun_family = AF_UNIX };
    strncpy(addr.sun_path, path, sizeof(addr.sun_path)-1);
    if (bind(server_fd,(struct sockaddr*)&addr,sizeof(addr))<0) { perror("stream bind"); exit(1); }
    if (listen(server_fd,5)<0) { perror("stream listen"); exit(1); }
    printf("UNIX Stream server listening on %s\n",path);

    for (;;) {
        int client = accept(server_fd,NULL,NULL);
        if (client<0) { perror("accept"); continue; }
        char buf[1024];
        int n = read(client,buf,sizeof(buf)-1);
        if (n>0) {
            buf[n]='\0';
            printf("Stream recv: %s\n",buf);
            /* handle exactly “ADD ATOM COUNT” here, identical to your TCP code */
            char cmd[16], atom[16]; unsigned cnt;
            int ok = sscanf(buf,"%15s%15s%u",cmd,atom,&cnt)==3
                  && strcmp(cmd,"ADD")==0;
            char resp[128];
            if (ok) {
              if      (!strcmp(atom,"HYDROGEN")) hydrogen+=cnt;
              else if (!strcmp(atom,"OXYGEN"))   oxygen   +=cnt;
              else if (!strcmp(atom,"CARBON"))   carbon   +=cnt;
              else ok=0;
              if (ok)
                snprintf(resp,sizeof(resp),
                         "Hydrogen: %u  Oxygen: %u  Carbon: %u\n",
                         hydrogen,oxygen,carbon);
            }
            if (!ok) strncpy(resp,"Error: Unknown or bad ADD\n",sizeof(resp));
            write(client,resp,strlen(resp));
        }
        close(client);
    }
    /* never reached */
    close(server_fd);
}

void start_unix_datagram(const char *path) {
    int server_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (server_fd < 0) { perror("dgram socket"); exit(1); }
    unlink(path);
    struct sockaddr_un addr = { .sun_family = AF_UNIX };
    strncpy(addr.sun_path, path, sizeof(addr.sun_path)-1);
    if (bind(server_fd,(struct sockaddr*)&addr,sizeof(addr))<0) { perror("dgram bind"); exit(1); }
    printf("UNIX Datagram server listening on %s\n",path);

    for (;;) {
        struct sockaddr_un client;
        socklen_t cl = sizeof(client);
        char buf[1024];
        int n = recvfrom(server_fd,buf,sizeof(buf)-1,0,(struct sockaddr*)&client,&cl);
        if (n<=0) continue;
        buf[n]='\0';
        printf("Dgram recv: %s\n", buf);
        /* handle “DELIVER MOLECULE COUNT” here, same as your UDP code */
        char cmd[16], mol[16], resp[128]; unsigned cnt;
        int ok = sscanf(buf,"%15s%15s%u",cmd,mol,&cnt)==3
              && strcmp(cmd,"DELIVER")==0;
        int success = 0;
        if (ok) {
          if      (!strcmp(mol,"WATER"))          success = create_h2o(cnt),          water           +=success?cnt:0;
          else if (!strcmp(mol,"CARBON_DIOXIDE")) success = create_carbon_dioxide(cnt),carbon_dioxide  +=success?cnt:0;
          else if (!strcmp(mol,"ALCOHOL"))        success = create_alcohol(cnt),       alcohol         +=success?cnt:0;
          else if (!strcmp(mol,"GLUCOSE"))        success = create_glucose(cnt),       glucose         +=success?cnt:0;
          else ok=0;
        }
        if (!ok) {
          snprintf(resp,sizeof(resp),"Error: bad DELIVER\n");
        } else {
          snprintf(resp,sizeof(resp),
                   "%s: %s x%u\n",
                   success?"DELIVERED":"FAILED", mol, cnt);
        }
        sendto(server_fd,resp,strlen(resp),0,
               (struct sockaddr*)&client,cl);
    }
    /* never reached */
    close(server_fd);
}
