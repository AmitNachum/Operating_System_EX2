CC = gcc
CFLAGS = -Wall -g
SERVER = atom_warehouse
CLIENT = atom_supplier

# Sources
SERVER_SRC = atom_warehouse.c
CLIENT_SRC = atom_supplier.c

.PHONY: all clean

all: $(SERVER) $(CLIENT)

$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER_SRC)

$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT_SRC)

clean:
	rm -f $(SERVER) $(CLIENT)
