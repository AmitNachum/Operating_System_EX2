# ——————————————————————————————
# Makefile for gcc --coverage (separate compile + link)
# ——————————————————————————————
CC        := gcc
CFLAGS    := --coverage -g -Wall -Wextra
LDFLAGS   := --coverage

CLIENT           := requester
SERVER           := supplier

# Sources
REQ_SRC          := molecular_requester.c
SUP_SRC          := molecular_suppliers.c
FORM_SRC         := formulas.c
GLOB_SRC         := ../Q_1/globals.c

# Objects (one .o per .c)
REQ_OBJ          := $(REQ_SRC:.c=.o)
SUP_OBJ          := $(SUP_SRC:.c=.o)
FORM_OBJ         := $(FORM_SRC:.c=.o)
GLOB_OBJ         := $(GLOB_SRC:.c=.o)

TCP_PORT         := 5555
UDP_PORT         := 6666

.PHONY: all clean coverage

all: $(CLIENT) $(SERVER)

# Compile each .c to .o + .gcno
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile globals.c (in parent dir) to globals.o + globals.gcno
../Q_1/%.o: ../Q_1/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link the requester
$(CLIENT): $(REQ_OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

# Link the server
$(SERVER): $(SUP_OBJ) $(FORM_OBJ) $(GLOB_OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	@echo "🧹 Cleaning up binaries, coverage files…and any server still bound on ports $(TCP_PORT)/tcp or $(UDP_PORT)/udp"
	# kill any process listening on TCP port
	-@fuser -k $(TCP_PORT)/tcp >/dev/null 2>&1
	# kill any process listening on UDP port
	-@fuser -k $(UDP_PORT)/udp >/dev/null 2>&1
	rm -f $(CLIENT) $(SERVER) \
	      *.o ../Q_1/*.o \
	      *.gcno *.gcda *.gcov \../Q_1/globals.gcno


coverage: clean all
	@echo "\n>>> Running coverage test…"
	@env COVERAGE_MODE=1 ./$(SERVER) $(TCP_PORT) $(UDP_PORT) & \
	sleep 1 && echo "DELIVER WATER 2" | ./$(CLIENT); \
	wait; \
	gcov -b molecular_suppliers.c formulas.c molecular_requester.c
