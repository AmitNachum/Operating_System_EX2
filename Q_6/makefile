CC = gcc
CFLAGS = -Wall -Wextra -g
OBJS = ../Q_1/globals.c ../Q_2/formulas.c ../Q_3/drinks.c

all: client_q6 server_q6

client_q6: client_q6.c
	$(CC) $(CFLAGS) -o client_q6 client_q6.c $(OBJS)

server_q6: server_q6.c
	$(CC) $(CFLAGS) -o server_q6 server_q6.c $(OBJS)

clean:
	rm -f client_q6 server_q6
