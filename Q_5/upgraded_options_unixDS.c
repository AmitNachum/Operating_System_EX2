#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "loading_servers.h"

/* your globals (exactly one definition) */
unsigned int hydrogen=0, oxygen=0, carbon=0;
unsigned int water=0, carbon_dioxide=0, alcohol=0, glucose=0;

static char *uds_stream=NULL, *uds_dgram=NULL;

void handle_timeout(int sig){
  (void)sig;
  printf("Timeout, shutting down…\n");
  if(uds_stream) unlink(uds_stream);
  if(uds_dgram ) unlink(uds_dgram);
  exit(0);
}

int main(int argc, char **argv){
  int tcp_port=-1, udp_port=-1;
  int o=0,c=0,h=0,timeout=-1;
  struct option longopts[] = {
    {"tcp-port",   1,0,'T'},
    {"udp-port",   1,0,'U'},
    {"stream-path",1,0,'s'},
    {"datagram-path",1,0,'d'},
    {"oxygen",     1,0,'o'},
    {"carbon",     1,0,'c'},
    {"hydrogen",   1,0,'h'},
    {"timeout",    1,0,'t'},
    {0,0,0,0}
  };
  int ch;
  while((ch=getopt_long(argc,argv,"T:U:s:d:o:c:h:t:",longopts,NULL))!=-1){
    switch(ch){
      case 'T': tcp_port=atoi(optarg); break;
      case 'U': udp_port=atoi(optarg); break;
      case 's': uds_stream=strdup(optarg); break;
      case 'd': uds_dgram =strdup(optarg); break;
      case 'o': oxygen   =atoi(optarg); break;
      case 'c': carbon   =atoi(optarg); break;
      case 'h': hydrogen =atoi(optarg); break;
      case 't': timeout  =atoi(optarg); break;
      default: fprintf(stderr,"Usage: %s -T <port> -U <port> [-s PATH] [-d PATH] [-oO2counts] [-t secs]\n",argv[0]);
               exit(1);
    }
  }
  if(tcp_port<0||udp_port<0){ fprintf(stderr,"-T and -U are mandatory\n"); exit(1); }

  if(timeout>0){
    signal(SIGALRM,handle_timeout);
    alarm(timeout);
  }

  pthread_t ts,td;
  if(uds_stream) pthread_create(&ts,NULL,(void*)start_unix_stream,uds_stream);
  if(uds_dgram ) pthread_create(&td,NULL,(void*)start_unix_datagram,uds_dgram);



  pthread_join(ts,NULL);
  pthread_join(td,NULL);
  return 0;
}
