#ifndef LOADING_SERVERS_H
#define LOADING_SEREVRS_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../Q_3/drinks.h"
#include <getopt.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "../Q_2/formulas.h"
#include "../Q_1/atoms.h"



void start_unix_stream(char* path);
void start_unix_datagram(char* path);





#endif