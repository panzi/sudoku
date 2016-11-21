CC=gcc
#CC=clang
CFLAGS=-Wall -Wextra -Werror -pedantic -std=c11 -O2 -pthread
#CFLAGS=-Wall -Wextra -Werror -std=gnu11 -O2 -pthread -g -DDEBUG
OBJ=sudoku.o

.PHONY: all clean

all: sudoku

sudoku: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

%.o: %.c panic.h
	$(CC) $(CFLAGS) $< -c -o $@

clean:
	rm -f $(OBJ) sudoku
