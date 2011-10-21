CC=gcc
CFLAGS=-O0 -ggdb3 -Wall -Wextra -Wno-unused-parameter -Wno-sign-compare -Werror
LIBS=-ltalloc -ltevent

all: tevent_sigint_switch

tevent_sigint_switch:
	mkdir bin
	$(CC) $(CFLAGS) $(LIBS) src/tevent_sigint_switch/tevent_sigint_switch.c -o bin/tevent_sigint_switch

clean:
	rm -fr bin
