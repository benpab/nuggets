# CS50 Team VI Project
## CS50 Spring 2024

### Client
The nuggets client module is contained in one file, used for the CS50 nuggets project. For the nuggets game, the _client_ provides a way to connect with the _server_, allowing players to join and play the game. 

The _client_ connects with the server via the `log.h` module, and communicates via UDP. Client sends information about the client to the server, and the server responds with game information for the client. The client uses the `ncurses.h` module to create a window that allows the information recieved from the server to be displayed, as well as take in new input to send to the server.

## Compiling

To compile,
```
    make
```

To clean,
```
    make clean
```

## Usage
In a typical use, there should be a subdirectory named `support` and a subdirectory named `modules`, within a directory where the main program is located.
The Makefile of that main directory might then include content like this:

To run `client`,
    ./client <HOSTNAME> <PORT> <USERNAME>

## Testing

The `clientTest` program tests the commandline arguments.

To run:
    chmod +x ./clientTest.sh
    ./clientTest.sh > clientTest.out

To test for functionality as well for memory errors, rigorous testing was done with the main `server.c` server files. No errors were found, and all leaks found were traced to the `ncurses.h` module.

### Assumptions

It is assumed that players are able to adjust screen size on their own if the map is too big for the current screen, but not over the max display size. It is apparent if the map is cut off. The screen will refresh, containing the entire map, on the next update from the server.