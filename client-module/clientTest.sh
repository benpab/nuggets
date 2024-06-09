#!/bin/bash

# clientTest.sh
# Group VI - CS50 S24
#
# test file for client.c
#
# Run with ./clientTest.sh > clientTest.out

echo -e "\n********\nTESTFILE OUTPUT FOR client\n********\n\ntesting start...\n\n"

#make sure files are recent
make clean


#testing for invalid parameters

echo -e "\ntest for only no args: \n"
./client
echo -e "\ntest for only 1 arg: \n"
./client plank
echo -e "\ntest for too many args: \n"
./client plank 00000 testname extra
echo -e "\ntest for invalid port: \n"
./client plank 00000 testname

echo -e "\n\ntesting completed!"