#ifndef FORMULAS_H
#define FORMULAS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/select.h>

extern unsigned int hydrogen;
extern unsigned int oxygen;
extern unsigned int carbon;
extern unsigned int water;
extern unsigned int carbon_dioxide;
extern unsigned int glucose;
extern unsigned int alcohol;

int create_h2o(unsigned int quantity);
int create_carbon_dioxide(unsigned int quantity);
int create_alcohol(unsigned int quantity);
int create_glucose(unsigned int quantity);

#endif