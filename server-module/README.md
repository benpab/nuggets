# CS50 Team VI Project
## CS50 Spring 2024

### Server Module

The server.c file contains the implementation for game logic and is essentially the central piece to the nuggets game. Server calls functions from the message, player and map module to properly implement the game as defined by REQUIREMENTS.

The server is meant to interact with the client as described in REQUIREMENTS. The server is prepared to receive messages of the type: PLAY SPECTATE and KEY, any other message will not get a response, but will be logged to stderr.

The overall flow of server is as follows:
    parse the commandline,
    if given valid commandline args:
        create the game by initializing a game
        print out the port to play on
        wait for messages
        when we receive a message:
            handle the message as required by REQUIREMENTS
            once all gold is collected:
                send a quit message with final summary 
                free everything and exit

This is the overall structure that server follows. For exact specifications for how this is implemented please see IMPLEMENTATION.md, as well each function has header information before its definition in server.c. No assumptions were made beyond the specification. We chose to not send an optional message back to the user if they give us an invalid message or keystroke.

## Compilation:
To compile simply, make 
To test simply, make test
To test with valgrind simply, make valgrind
To clean simply, make clean

## Errors and Logging:

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status.

Once the program is running, memory related errors will cause significant issues in the game with respect to the map being displayed properly and proper game behavior. In order to avoid game breaking bugs, when we cannot properly allocate space, we log an error message and exit with a non-zero status. 

### Testing

The `servertest.c` program tests the commandline arguments as well as creating and freeing the necessary structures. When this is run with valgrind (make valgrind) we see that there are no warnings, no errors or segfaults.

To test, simply `make test`.
See testing.out for details of testing and an example test run.
