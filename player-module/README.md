# CS50 Nuggets - Player Module
## Team IV
## CS50 Spring 2024

### Player

A `player` is a character for the user to act as in the nuggets game. 
The `player` stores the game information for the player, such as his location, playerID and gold collected.
The player is then added to a hashtable in the game state that stores all players in the game.

### Usage

The *player* module, defined in `player.h` and implemented in `player.c`, exports the following functions:

```c
bool initializePlayer(gameState_t* currState, char* playerName, addr_t playerAddress);
bool updateLineOfSight(map_t* masterMap, map_t* visibleMap, player_t* player, gameState_t* currstate);
bool movePlayer(gameState_t* currState, player_t* player, char* keystroke);
void playerQuit(player_t* player);
void playerDelete(void* item);
void playerPrint(player_t* player);
```

Additionally, the module has helper functions which are defined and brifely explained in player.c

### Implementation

The *player* itself is represented as a `struct player` defined in the corestructures.h file;
A player is initialize by passing a gamestate, a name and a player address of type addr_t. The function the initialize all instance variables to their correct values and places the player in a random location in the map.
The player is then added to the hashtable of all players in the gamestate.

The `updateLineOfsight` method calculates and updates the player's current visibility using Bresenham's line algorithm, clearing all non-visible coordinates in the map, making them invisible.

The `move_player` method checks which key has been passed as a parameter and relocates the player to a new location in the map, depending on the key passed. If a key passed is invalid or the new location is invalid the player will not be relocated and the function will return false.

The `playerQuit` method takes care of the case when key `q` is passed into the movePlayer function, and marks the player as inactive.

The `playerDelete` method frees each necessary instance variable in the player. It concludes by freeing the `struct player`. This function takes `void* item` as a parameter since it is called with the function `hashtable_delete` in the hashtable module in the game's main function.

The `playerPrint` method prints out the player's current information, including his `playerID`, `playerLocation`, and `playerAddress`. This function is not used in the game itself but is important during the testing process.

### Assumptions

We assume that the gamestate passed into each function is always a valid gamestate.

### Files

* `Makefile` - compilation procedure
* `player.h` - the interface
* `player.c` - the implementation
* `playertest.c` - unit test driver
* `testing.out` - Output from our test file

### Compilation

To compile, simply `make player.o`.

### Testing

The `playertest.c` program tests all error and edge cases as well as working cases such as players moving and crashing into each other.
It tests all potential error and edge cases.

To test, simply `make test`.
See `testing.out` for details of testing and an example test run.

To test with valgrind, `make valgrind`.