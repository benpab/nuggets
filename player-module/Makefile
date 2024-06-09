# Makefile for the player module
# Team VI, Spring 2024

M = ../map-module
L = ../support
OBJS = playertest.o player.o $M/map.o $L/log.o $L/message.o 
LIBS = $L/libcs50-given.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L -I$M
# LFLAGS = -L../libcs50 -L../common 
CC = gcc
MAKE = make
# for memory-leak tests 
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

.PHONY: playertest

all: playertest

playertest.o: $L/hashtable.h ../corestructures.h $M/map.h player.h $L/log.h $L/message.h 
map.o: $M/map.h ../corestructures.h $M/file.h 
player.o: player.h ../corestructures.h $M/map.h 
log.o: $L/log.h 
message.o: $L/log.h $L/message.o 

playertest: $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(LFLAGS) $(OBJS) $(LIBS) -o playertest  

test: playertest ../maps/main.txt
	./playertest ../maps/main.txt &> testing.out

valgrind: playertest
	$(VALGRIND) ./playertest ../maps/main.txt

clean: 

	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f playertest
	rm -f core