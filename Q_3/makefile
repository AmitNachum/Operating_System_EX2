CC = gcc
CFLAGS = -I../Q_2 -Wall -Wextra -g

SERVER_SRC = drinks_bar.c drinks.c
CLIENT_SRC = client_for_drink.c

SERVER_BIN = drinks_bar
CLIENT_BIN = drinks_client

.PHONY: all clean run-server run-client

all: $(SERVER_BIN) $(CLIENT_BIN)

$(SERVER_BIN): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER_BIN) $(SERVER_SRC) ../Q_2/formulas.c

$(CLIENT_BIN): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT_BIN) $(CLIENT_SRC)

run-server: $(SERVER_BIN)
	./$(SERVER_BIN) 5555 6666

run-client: $(CLIENT_BIN)
	./$(CLIENT_BIN)

clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN)
