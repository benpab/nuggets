# Makefile for Ben's dev branch
#
# Ben Pable
# CS50 - S24

# object files
LIBS = ../support/support.a
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I../support
MAKE = make

.PHONY: clean client test all

all: client

# dependencies
client: client.o $(LIBS)
	$(CC) $(CFLAGS) $^ -o $@ -lncurses

client.o: ../support/message.h

test: 
	chmod +x clientTest.sh
	./clientTest.sh &> testing.out 

clean:
	rm -f *~ *.o *.dSYM
	rm -f client
	rm -rf data/