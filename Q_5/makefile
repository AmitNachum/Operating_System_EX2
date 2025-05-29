#—————————————————————————————————————
# Makefile — Step 5: UDS support + Code Coverage
#—————————————————————————————————————
SHELL      := /bin/bash
CC         := gcc

# Instrument both compile _and_ link
CFLAGS     := --coverage -g -Wall -Wextra -I../Q_3 -I../Q_2 -I../Q_1
LDFLAGS    := --coverage

# default ports & socket paths
TCP_PORT   := 5555
UDP_PORT   := 6666
UDS_STREAM := /tmp/uds_stream.sock
UDS_DGRAM  := /tmp/uds_dgram.sock
CLI_DGRAM  := /tmp/cli_uds_dgram.sock

# Binaries
SERVER     := drinks_bar
CLIENT_REG := client_regular
CLIENT_UDS := client_uds

# Sources
SRV_SRC    := upgraded_options_unixDS.c         \
              ../Q_3/drinks.c                   \
              ../Q_2/formulas.c                 \
              ../Q_1/globals.c

CLI_REG_SRC:= client_options_regular.c
CLI_UDS_SRC:= client_upgraded_options.c

# Objects
SRV_OBJS   := $(SRV_SRC:.c=.o)
CLI_REG_O  := client_options_regular.o
CLI_UDS_O  := client_upgraded_options.o

.PHONY: all clean run-server run-client-regular run-client-uds coverage

all: $(SERVER) $(CLIENT_REG) $(CLIENT_UDS)

#------------------------
# Compile rules (instrumented)
#------------------------
$(SRV_OBJS): %.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(CLI_REG_O): client_options_regular.c
	$(CC) $(CFLAGS) -c $< -o $@

$(CLI_UDS_O): client_upgraded_options.c
	$(CC) $(CFLAGS) -c $< -o $@

#------------------------
# Link rules (pull in libgcov)
#------------------------
$(SERVER): $(SRV_OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(CLIENT_REG): $(CLI_REG_O)
	$(CC) $(LDFLAGS) -o $@ $^

$(CLIENT_UDS): $(CLI_UDS_O)
	$(CC) $(LDFLAGS) -o $@ $^

#------------------------
# Clean up everything
#------------------------
clean:
	@echo "🧹 Cleaning binaries, objects, coverage, killing ports/sockets…"
	-@fuser -k $(TCP_PORT)/tcp >/dev/null 2>&1
	-@fuser -k $(UDP_PORT)/udp >/dev/null 2>&1
	-@rm -f $(UDS_STREAM) $(UDS_DGRAM) $(CLI_DGRAM)
	@rm -f $(SERVER) $(CLIENT_REG) $(CLIENT_UDS) \
	      *.o ../Q_3/*.o ../Q_2/*.o ../Q_1/*.o \
	      *.gcno *.gcda *.gcov ../Q_3/drinks.gcno ../Q_2/formulas.gcno ../Q_1/globals.gcno ../Q_3/drinks.gcda ../Q_2/formulas.gcda ../Q_1/globals.gcda

#------------------------
# Convenience run targets
#------------------------
run-server: $(SERVER)
	@echo "▶️  Starting server:"
	@echo "   • TCP/UDP fallback:   -T $(TCP_PORT) -U $(UDP_PORT)"
	@echo "   • UDS-STREAM socket:  -s $(UDS_STREAM)"
	@echo "   • UDS-DGRAM  socket:  -d $(UDS_DGRAM)"
	./$(SERVER) \
	  -s $(UDS_STREAM) -d $(UDS_DGRAM) \
	  -T $(TCP_PORT)  -U $(UDP_PORT) \
	  -o 5 -c 3 -h 7 -t 20

run-client-regular: $(CLIENT_REG)
	@echo "▶️  TCP/UDP client → localhost:$(TCP_PORT)"
	./$(CLIENT_REG)

run-client-uds: $(CLIENT_UDS)
	@echo "▶️  UDS client → stream:$(UDS_STREAM) or dgram:$(UDS_DGRAM)"
	@echo "   e.g. ./$(CLIENT_UDS) -f $(UDS_STREAM)"
	./$(CLIENT_UDS) -f $(UDS_STREAM)

#------------------------
# Coverage target
#------------------------

coverage: clean all
	@echo "\n>>> Running coverage for Q_5 (UDS‐stream)…"

	@env COVERAGE_MODE=1 ./$(SERVER)                                      \
	  -s $(UDS_STREAM)                                                    \
	  -T 0 -U 0                                                          \
	  -o 100 -c 100 -h 100 -t 10                                                \
	  &                                                                   \

	sleep 1

	@printf "ADD HYDROGEN 1\n"                                             \
	  | ./$(CLIENT_UDS) -f $(UDS_STREAM)

	wait
	
	gcov -b -c                                                             \
	  upgraded_options_unixDS.c                                           \
	  ../Q_3/drinks.c                                                     \
	  ../Q_2/formulas.c                                                   \
	  ../Q_1/globals.c                                                    \
	  client_upgraded_options.c