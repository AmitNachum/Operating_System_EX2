#ifndef DRINKS_H
#define DRINKS_H
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
#include "../Q_2/formulas.h"
#include "../Q_1/atoms.h"

unsigned int min3(unsigned int a, unsigned int b, unsigned int c);
int count_soft_drink();
int count_vodka();
int count_champagne();
int make_soft_drink(unsigned int quantity);
int make_vodka(unsigned int quantitiy);
int make_champagne(unsigned int quantity);



#endif