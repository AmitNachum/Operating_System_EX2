CC = gcc
CFLAGS =-I../Q_3../Q_2 -Wall -Wextra -g

# Targets
all: client_options drinks_options_bar

client_options: client_options.c
	$(CC) $(CFLAGS) -o client_options client_options.c
	

drinks_options_bar: drinks_options_bar.c
	$(CC) $(CFLAGS) -o drinks_options_bar drinks_options_bar.c ../Q_3/drinks.c ../Q_2/formulas.c

# Cleanup
clean:
	rm -f client_options drinks_options_bar
