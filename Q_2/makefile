CC = gcc
CFLAGS =  -I../Q_2 -Wall -Wextra -g

CLIENT_SRC = molecular_requester.c
SERVER_SRC = molecular_suppliers.c formulas.c

CLIENT_BIN = requester
SERVER_BIN = supplier

.PHONY: all clean

all: $(CLIENT_BIN) $(SERVER_BIN)

$(CLIENT_BIN): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT_BIN) $(CLIENT_SRC)

$(SERVER_BIN): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER_BIN) $(SERVER_SRC)

run-client: $(CLIENT_BIN)
	./$(CLIENT_BIN)

run-server: $(SERVER_BIN)
	./$(SERVER_BIN) 5555 6666

clean:
	rm -f $(CLIENT_BIN) $(SERVER_BIN)
