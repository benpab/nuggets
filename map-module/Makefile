# Makefile for the player module
# Team VI, Spring 2024

L = ../support
P = ../player-module
OBJS = mapTest.o map.o $P/player.o $L/log.o $L/message.o 
LIBS = $L/libcs50-given.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L -I$P
# LFLAGS = -L../libcs50 -L../common 
CC = gcc
MAKE = make
# for memory-leak tests 
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

.PHONY: mapTest

all: mapTest

mapTest.o: $L/hashtable.h ../corestructures.h map.h $P/player.h $L/log.h $L/message.o 
map.o: map.h ../corestructures.h file.h 
player.o: $P/player.h ../corestructures.h map.h 
log.o: $L/log.h 
message.o: $L/log.h $L/message.o 

mapTest: $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(LFLAGS) $(OBJS) $(LIBS) -o mapTest  

test: mapTest ../maps/main.txt
	./mapTest ../maps/main.txt &> testing.out

valgrind: mapTest
	$(VALGRIND) ./mapTest ../maps/main.txt

clean: 

	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f mapTest
	rm -f core