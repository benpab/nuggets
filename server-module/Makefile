# Makefile for server module
#
# Team VI, Spring 2024

L = ../support
P = ../player-module
M = ../map-module
S = server-module
C = ../client-module

OBJS = server.o $P/player.o $M/map.o $L/log.o $L/message.o 
TOBJS = servertest.o $P/player.o $M/map.o $L/log.o $L/message.o 
COBJS = $C/client.o $L/message.o
LIBS = $L/libcs50-given.a 
CLIBS = -lncurses support/support.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L -I$P -I$M
# LFLAGS = -L../libcs50 -L../common 
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

.PHONY: server

all: 
	make server 

server.o: $L/hashtable.h ../corestructures.h $M/map.h $P/player.h $L/log.h $L/message.o 
map.o: $M/map.h ../corestructures.h $M/file.h 
player.o: $P/player.h ../corestructures.h $M/map.h 
log.o: $L/log.h 
message.o: $L/log.h $L/message.o 
servertest.o:  $L/hashtable.h ../corestructures.h $M/map.h $P/player.h $L/log.h $L/message.o 

# servertest.o: server.h

server: $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(LFLAGS) $(OBJS) $(LIBS) -o server    

servertest: $(TOBJS)  $(LIBS)
	$(CC) $(CFLAGS) $(LFLAGS) $(TOBJS)  $(LIBS) -o servertest

test: servertest 
	./servertest 2>&1 &> testing.out

valgrind: servertest 
	$(VALGRIND) ./servertest 2>&1 &> testing.out

clean: 
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f server
	rm -f servertest
