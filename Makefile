# Makefile for Ben's dev branch
#
# Team VI, Spring 2024

L = support
P = player-module
M = map-module
S = server-module
C = client-module

OBJS = $S/server.o $P/player.o $M/map.o $L/log.o $L/message.o 
COBJS = $C/client.o $L/message.o
LIBS = $L/libcs50-given.a 
CLIBS = -lncurses support/support.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L -I$P -I$M
# LFLAGS = -L../libcs50 -L../common 
CC = gcc
CFLAGS = -Wall -pedantic -std=c11 -ggdb -I./support
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

.PHONY: server

all: 
	make -C support
	make -C player-module
	make -C map-module
	make -C server-module
	make -C client-module
	
test: 
	make -C player-module test
	make -C map-module test
	make -C server-module test
	make -C client-module test

clean: 
	make -C player-module clean
	make -C map-module clean
	make -C server-module clean
	make -C client-module clean
	make -C support clean
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f server
	rm -f client