#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char **argv){
  char *uds_path=NULL, *host="127.0.0.1";
  int port=5555, fd;
  struct option opts[] = {
    {"uds", required_argument,0,'f'},
    {"host",required_argument,0,'h'},
    {"port",required_argument,0,'p'},
    {0,0,0,0}
  };
  int c;
  while((c=getopt_long(argc,argv,"f:h:p:",opts,NULL))!=-1){
    if (c=='f') uds_path=optarg;
    if (c=='h') host=optarg;
    if (c=='p') port=atoi(optarg);
  }
  if (uds_path) {
    struct sockaddr_un a={.sun_family=AF_UNIX};
    strncpy(a.sun_path,uds_path,sizeof(a.sun_path)-1);
    fd = socket(AF_UNIX,SOCK_STREAM,0);
    if (connect(fd,(struct sockaddr*)&a,sizeof(a))<0){ perror("connect");exit(1);}
  } else {
    struct addrinfo h={.ai_family=AF_INET,.ai_socktype=SOCK_STREAM}, *r;
    char ps[6]; snprintf(ps,sizeof(ps),"%d",port);
    if(getaddrinfo(host,ps,&h,&r)) { perror("gai"); exit(1); }
    fd=socket(r->ai_family,r->ai_socktype,r->ai_protocol);
    connect(fd,r->ai_addr,r->ai_addrlen);
    freeaddrinfo(r);
  }

  char line[128];
  while(printf("> "), fflush(stdout), fgets(line,sizeof(line),stdin)){
    if (!strncmp(line,"exit",4)) break;
    if (strncmp(line,"ADD",3)){ 
      puts("Usage: ADD <HYDROGEN|OXYGEN|CARBON> <N>"); 
      continue; 
    }
    send(fd,line,strlen(line),0);
    int n=recv(fd,line,sizeof(line)-1,0);
    if(n>0){ line[n]=0; printf("Server: %s",line); }
  }
  close(fd);
  return 0;
}
