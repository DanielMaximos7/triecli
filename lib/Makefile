CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -g -O0
SAN ?= -fsanitize=address,undefined

all: test_radix

test_radix: test_radix.c RadixTree.c RadixTree.h
	$(CC) $(CFLAGS) $(SAN) -o $@ test_radix.c RadixTree.c

clean:
	rm -f test_radix

.PHONY: all clean
