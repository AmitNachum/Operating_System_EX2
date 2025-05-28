CC = gcc
CFLAGS = -Wall -g -fprofile-arcs -ftest-coverage
LDFLAGS = -fprofile-arcs -ftest-coverage
SERVER = atom_warehouse
CLIENT = atom_supplier

# Sources
SERVER_SRC = atom_warehouse.c
CLIENT_SRC = atom_supplier.c

.PHONY: all clean coverage

all: $(SERVER) $(CLIENT)

$(SERVER): $(SERVER_SRC)
	$(CC) $(CFLAGS) $(SERVER_SRC) -o $(SERVER) $(LDFLAGS)

$(CLIENT): $(CLIENT_SRC)
	$(CC) $(CFLAGS) $(CLIENT_SRC) -o $(CLIENT) $(LDFLAGS)

clean:
	rm -f $(SERVER) $(CLIENT) *.gcno *.gcda *.gcov

coverage: all
	./$(SERVER) 5555 & \
	SERVER_PID=$$!; \
	sleep 1; \
	echo "ADD HYDROGEN 2" | ./$(CLIENT) 5555; \
	wait $$SERVER_PID; \
	gcov $(SERVER_SRC) $(CLIENT_SRC)
