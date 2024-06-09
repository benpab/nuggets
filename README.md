# Nuggets
## Team VI 
### Spring 2024

### Team Names:
Cameron MacDonald, Kevin Luo, Oskar Magnusson, Ben Pable

### Explanation
This repository contains the code for the CS50 "Nuggets" game, in which players explore a set of rooms and passageways in search of gold nuggets.
The rooms and passages are defined by a *map* loaded by the server at the start of the game.
The gold nuggets are distributed in piles into random spots within the rooms.
Up to 26 *players*, and one spectator, may play a given game.
Each *player* is randomly dropped into a room when joining the game.
Players move about, collecting nuggets when they move onto a pile.
When all gold nuggets are collected, the game ends and a summary is printed.

## Functionality
We have implemented all functionality according to REQUIREMENTS.md.
We did not complete any extra credit.

## To compile and run
To compile: make
To run the server: server-module/./server maps/main.txt <seed>
To run the client: client-module/./client <server> <port> <name>
To run testing code: make test

### libcs50-given library
The only .a file that we push to our branch in git is the libcs50-given.a library which was given to us in a previous lab. This is needed for the game to run, since we use hashtables in our functionality.

## Materials provided

See the [support library](support/README.md) for some useful modules.

See the [maps](maps/README.md) for some draft maps.

See the [map](map/README.md) for detailed explanation of the map module

See the [player](player/README.md) for detailed explanation of the player module

See the [client](client-module/README.md) for detailed explanation of the client module

See the [server](server-module/README.md) for detailed explanation of the server module

See corestructures.h for the definition of structures used in the code

### Subdirectories
* `support` - given support module with extra cs50-lib support
* `maps` - given maps modules for txt files
* `map` - The map subsystem
* `player` - The player subsystem
* `client` - The client subsystem
* `server` - The server subsystem

### Scrum Explanation
We followed scrum by having daily scrum meetings to go over goals and progress and any potential road blocks. We used GitHub project board but not as we completed the work. Instead we had increased communication through slack, in person coding sessions, and our scrum documentation. 

### Git Flow Explanation 
We all worked on our own branches and when we had working code we pushed to our own ready branches. The server-logic branch acted as a development branch where only code that seemed to work was pushed. This is due to the nature of our implementation as server depended on both map and player module. This was also because map and player could be tested indivually so we made sure all code was tested extensively in its individual module before being brought into server-logic. When had a game that was functional (met enough requirements) we pushed it to main. This was to ensure main only ever had a game that we would be satisfied with a potential client playing. 