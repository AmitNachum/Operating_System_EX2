SHELL    := /bin/bash
CC       := gcc
CFLAGS   := --coverage -g -Wall -Wextra -I../Q_3 -I../Q_2 -I../Q_1
LDFLAGS  := --coverage

TCP_PORT := 5555
UDP_PORT := 6666

SERVER   := drinks_options_bar
CLIENT   := client_options

# exactly one object per source
SRV_OBJS := drinks_options_bar.o drinks.o formulas.o globals.o
CLI_OBJS := client_options.o

.PHONY: all clean run-server run-client coverage

all: $(SERVER) $(CLIENT)

#— compile the bar’s main + headers in Q_4
drinks_options_bar.o: drinks_options_bar.c ../Q_3/drinks.h
	$(CC) $(CFLAGS) -c $< -o $@

#— compile the drinks logic from Q_3
drinks.o: ../Q_3/drinks.c ../Q_3/drinks.h
	$(CC) $(CFLAGS) -c $< -o $@

#— compile the formulas logic from Q_2
formulas.o: ../Q_2/formulas.c ../Q_2/formulas.h
	$(CC) $(CFLAGS) -c $< -o $@

#— compile the globals definitions from Q_1
globals.o: ../Q_1/globals.c ../Q_2/formulas.h
	$(CC) $(CFLAGS) -c $< -o $@

#— compile the options-aware client
client_options.o: client_options.c
	$(CC) $(CFLAGS) -c $< -o $@

#— link the server
$(SERVER): $(SRV_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(SRV_OBJS)

#— link the client
$(CLIENT): $(CLI_OBJS)
	$(CC) $(LDFLAGS) -o $@ $(CLI_OBJS)

clean:
	@echo "🧹 Cleaning bins, objs, coverage + killing ports..."
	-@fuser -k $(TCP_PORT)/tcp >/dev/null 2>&1
	-@fuser -k $(UDP_PORT)/udp >/dev/null 2>&1
	rm -f $(SERVER) $(CLIENT) \
	      *.o ../Q_3/*.o ../Q_2/*.o ../Q_1/*.o \
	      *.gcno *.gcda *.gcov ../Q_3/drinks.gcno ../Q_2/formulas.gcno ../Q_1/globals.gcno

run-server: $(SERVER)
	@echo "▶️  Server on TCP $(TCP_PORT), UDP $(UDP_PORT)"
	./$(SERVER) -T $(TCP_PORT) -U $(UDP_PORT)

run-client: $(CLIENT)
	@echo "▶️  Client → localhost:$(TCP_PORT)"
	./$(CLIENT) -h 127.0.0.1 -p $(TCP_PORT)

coverage: clean all
	@echo ">>> exercising coverage for Q_4…"

	@timeout 2s env COVERAGE_MODE=1 ./$(SERVER) \
		-T $(TCP_PORT) -U $(UDP_PORT) -o 2 -c 1 -h 3 -t 60 \
		< <(printf "GEN SOFT DRINK\nGEN VODKA\nGEN CHAMPAGNE\n") &


	@sleep 1


	@printf "ADD HYDROGEN 10\nADD OXYGEN 10\nADD CARBON 10\nDELIVER WATER 2\n" \
		| ./$(CLIENT) -h 127.0.0.1 -p $(TCP_PORT)

	@wait

	@gcov -b -c \
		drinks_options_bar.c \
		../Q_3/drinks.c \
		../Q_2/formulas.c \
		../Q_1/globals.c \
		client_options.c

