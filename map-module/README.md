# CS50 Team VI Project
## CS50 Spring 2024

### Map Module

The `map` module is designed to handle all aspects of the game's map. It implements a new struct `map` that stores the game's map as a 2D array of char*. This module is involved in initializing the start of the game by converting a map txt file in a `map` struct and loading gold randomly. The map module also provides vital functions used by the player module such as finding a random location for the player to spawn and getting/setting locations the player moves to. Utilizes `player` struct and `gameState` struct as well as functions from `file.c`.

### Usage

The *map* module, implemented in `map.c`, includes the following key functions:

```c
map_t* loadMap(const char* mapTxtFile);
void mapDelete(map_t* map);
void loadGold(map_t* map);
int* getRandomLocation(map_t* map);
char* convertMap(map_t* map, char* playerID);
char* getLocation(map_t* map, int x, int y);
void setLocation(map_t* map, int x, int y, const char* newTile);
map_t* copyMap(map_t* original);
void replaceGoldSpots(map_t* map, gameState_t* currState);
```

### Implementation

The Map module is implemented across two files: `map.c` and `map.h`, providing a range of functionalities:

- `loadMap`: Initializes and returns a new map structure based on a text file.
- `mapDelete`: Frees all memory associated with a given map.
- `void loadGold(map_t* map);` Populates the map with gold in random locations.
- `getRandomLocation`: Returns a random location within the map that is not currently occupied and is a valid spawn location
- `convertMap`: Converts the map into a string format for a specific player
- `getLocation`: Retrieves the tile at a specific coordinate in the map.
- `setLocation`: Updates the tile at a specified coordinate in the map.
- `copyMap`: Creates a copy of a given map with the same dimensions but will all empty spots
- `replaceGoldSpots`: Replaces gold spots on the map with empty room spots and spots players were on with empty room spotsor corridor spots, adjusting based on what the tile used to be.

### Assumptions
As stated in the requirement spec we assume the map that is given to the map module is a valid map.


### Files
`Makefile` - Manages compilation.
`map.c` - Main implementation of the map functions.
`map.h` - Header file defining the structures and declarations for map operations.
`mapTest.c` - Testing file for map module

### Compilation
To compile, simply run make within the map module

### Testing

<<<<<<< HEAD:modules/README.md
The map module is tested with the mapTest.c file. It tests all functions in the map module. To test it we can start off with smaller, simplier maps and slowly build up to more extensive maps



=======
The map module is tested with the mapTest.c file. It tests all functions in the map module. To test it we can start off with smaller, simplier maps and slowly build up to more extensive maps
>>>>>>> db934489f366d374c0e18eddf623a0e642671f98:map-module/README.md
