SHELL    := /bin/bash
CC       := gcc

# Turn on coverage for both compile and link
CFLAGS   := --coverage -g -Wall -Wextra \
            -I../Q_3 -I../Q_2 -I../Q_1
LDFLAGS  := --coverage

# Default ports and persistent‐store path
TCP_PORT := 5555
UDP_PORT := 6666
SAVE_F   := Atom_supply.txt

# Binaries
SERVER   := server_q6
CLIENT   := client_q6

# Sources
SRV_SRC  := server_q6.c      \
            ../Q_3/drinks.c  \
            ../Q_2/formulas.c\
            ../Q_1/globals.c
CLI_SRC  := client_q6.c

# Objects
SRV_OBJS := server_q6.o drinks.o formulas.o globals.o
CLI_OBJ  := client_q6.o

.PHONY: all clean run-server run-client coverage

all: $(SERVER) $(CLIENT)

# compile
server_q6.o: server_q6.c
	$(CC) $(CFLAGS) -c $< -o $@
drinks.o: ../Q_3/drinks.c
	$(CC) $(CFLAGS) -c $< -o $@
formulas.o: ../Q_2/formulas.c
	$(CC) $(CFLAGS) -c $< -o $@
globals.o: ../Q_1/globals.c
	$(CC) $(CFLAGS) -c $< -o $@
client_q6.o: client_q6.c
	$(CC) $(CFLAGS) -c $< -o $@

# link
$(SERVER): $(SRV_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^
$(CLIENT): $(CLI_OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

# clean up
clean:
	@echo "🧹 Killing servers on $(TCP_PORT)/tcp or $(UDP_PORT)/udp, cleaning up…"
	-@fuser -k $(TCP_PORT)/tcp >/dev/null 2>&1
	-@fuser -k $(UDP_PORT)/udp >/dev/null 2>&1
	@rm -f $(SERVER) $(CLIENT) \
	      *.o ../Q_3/*.o ../Q_2/*.o ../Q_1/*.o \
	      *.gcno *.gcda *.gcov \
	      $(SAVE_F)

# run targets
run-server: $(SERVER)
	@echo "▶️  Starting server on TCP $(TCP_PORT), UDP $(UDP_PORT)"
	./$(SERVER) -T $(TCP_PORT) -U $(UDP_PORT) -f $(SAVE_F) -t 0

run-client: $(CLIENT)
	@echo "▶️  Running client against localhost:$(TCP_PORT)"
	./$(CLIENT)

# coverage
coverage: clean all
	@echo "\n>>> Exercising persistence & gathering coverage…"
	@timeout 1s env COVERAGE_MODE=1 ./$(SERVER) \
	  -T $(TCP_PORT) -U $(UDP_PORT) \
	  -f $(SAVE_F) -t 100 -o 1000 -h 1000 -c 1000 \
	  </dev/null &

	@printf "\
	@sleep 2 \
DELIVER WATER 25\n\
    @sleep 2 \
DELIVER CARBON_DIOXIDE 34\n\
    @sleep 2 \
DELIVER ALCOHOL 10\n\
    @sleep 2 \
DELIVER GLUCOSE 2\n\
exit\n" | ./$(CLIENT)

	@wait

	@echo; echo "=== $(SAVE_F) after deliveries ==="; cat $(SAVE_F)

	@gcov -b -c \
	  server_q6.c \
	  ../Q_3/drinks.c \
	  ../Q_2/formulas.c \
	  ../Q_1/globals.c \
	  client_q6.c
