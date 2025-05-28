#————————————————————————————————————————————
# Makefile — Step 3 with Code Coverage
#————————————————————————————————————————————
CC         := gcc
CFLAGS     := --coverage -g -Wall -Wextra
LDFLAGS    := --coverage

TCP_PORT   := 5555
UDP_PORT   := 6666

SERVER     := drinks_bar
CLIENT     := client_for_drink

# Sources
DRINK_SRCS := drinks_bar.c drinks.c
FORM_SRC   := ../Q_2/formulas.c
GLOB_SRC   := ../Q_1/globals.c
CLIENT_SRC := client_for_drink.c

# Objects
DRINK_OBJS := $(DRINK_SRCS:.c=.o)
FORM_OBJ   := $(FORM_SRC:.c=.o)
GLOB_OBJ   := $(GLOB_SRC:.c=.o)
CLIENT_OBJ := $(CLIENT_SRC:.c=.o)

.PHONY: all clean run-server run-client coverage

all: $(SERVER) $(CLIENT)

# — Compile each local .c → .o + .gcno
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# — Compile Q_2/formulas.c → ../Q_2/formulas.o + .gcno
../Q_2/%.o: ../Q_2/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# — Compile Q_1/globals.c → ../Q_1/globals.o + .gcno
../Q_1/%.o: ../Q_1/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# — Link server (includes globals & formulas objs)
$(SERVER): $(DRINK_OBJS) $(FORM_OBJ) $(GLOB_OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

# — Link client
$(CLIENT): $(CLIENT_OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	@echo "🧹 Cleaning bins, objs, coverage and killing ports..."
	-@fuser -k $(TCP_PORT)/tcp >/dev/null 2>&1
	-@fuser -k $(UDP_PORT)/udp >/dev/null 2>&1
	# local objects + coverage
	rm -f $(SERVER) $(CLIENT) \
	      *.o *.gcno *.gcda *.gcov
	# Q_2: formulas
	rm -f ../Q_2/*.o ../Q_2/*.gcno ../Q_2/*.gcda
	# Q_1: globals
	rm -f ../Q_1/*.o ../Q_1/*.gcno ../Q_1/*.gcda


run-server: $(SERVER)
	@echo "▶️  Starting drinks_bar on TCP $(TCP_PORT), UDP $(UDP_PORT)…"
	./$(SERVER) $(TCP_PORT) $(UDP_PORT)

run-client: $(CLIENT)
	@echo "▶️  Starting client_for_drink…"
	./$(CLIENT)

coverage: clean all
	@echo "\n>>> Running coverage test…"
	@env COVERAGE_MODE=1 \
	 sleep 1 && printf "GEN SOFT DRINK\nGEN VODKA\nGEN CHAMPAGNE\n" | ./$(SERVER) $(TCP_PORT) $(UDP_PORT) \
	 ./$(CLIENT) \
	 wait; \
	 gcov -b -c drinks_bar.c drinks.c ../Q_2/formulas.c ../Q_1/globals.c client_for_drink.c
