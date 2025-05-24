CC = gcc
CFLAGS = -Wall -Wextra -g -I../Q_1 -I../Q_2 -I../Q_3

all: client server

client: client_upgraded_options.o ../Q_2/formulas.o ../Q_3/drinks.o
	$(CC) $(CFLAGS) -o $@ $^

server: upgraded_options_unixDS.o loading_servers.o ../Q_1/global.o ../Q_2/formulas.o ../Q_3/drinks.o
	$(CC) $(CFLAGS) -o $@ $^

# compile any .c in this dir
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# bring in the shared libraries from other exercises
../Q_1/global.o:
	$(CC) $(CFLAGS) -c ../Q_1/global.c -o $@

../Q_2/formulas.o:
	$(CC) $(CFLAGS) -c ../Q_2/formulas.c -o $@

../Q_3/drinks.o:
	$(CC) $(CFLAGS) -c ../Q_3/drinks.c -o $@

clean:
	rm -f *.o client server ../Q_1/global.o ../Q_2/formulas.o ../Q_3/drinks.o
