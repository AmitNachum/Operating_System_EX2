CC=gcc
CFLAGS=-Wall -Wextra -g

# Targets
all: client_q5 server_q5

client_q5: client_upgraded_options.c
	$(CC) $(CFLAGS) -o client_q5 client_upgraded_options.c

server_q5: upgraded_options_unixDS.c ../Q_1/globals.c ../Q_2/formulas.c ../Q_3/drinks.c
	$(CC) $(CFLAGS) -o server_q5 upgraded_options_unixDS.c ../Q_1/globals.c ../Q_2/formulas.c ../Q_3/drinks.c

clean:
	rm -f client_q5 server_q5
	rm -f /tmp/uds_stream.sock /tmp/uds_dgram.sock /tmp/cli_uds_dgram.sock
