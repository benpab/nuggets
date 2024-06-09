/*
* CS50 Nuggets
* Map Module Header File
* Team VI, Spring, 2024
*/

#ifndef __MAP_H
#define __MAP_H

#include "../corestructures.h"
#include "../player-module/player.h"
#include "../support/hashtable.h"

/**************** global types ****************/
typedef struct map map_t;

/**************** loadMap ****************/
/*
* Turns a map text file into a map struct
*
* Caller provides:
*   A valid map text file
* We return:
*   Pointer to a map struct
* We guarantee:
*   The map is a 2D array of char* that represents the same thing as the map text file
* Caller is responsible for:
*   later freeing the map
*/
map_t* loadMap(const char* mapTxtFile);

/**************** mapDelete ****************/
/*
* Frees all the memory associated with a map
*
* Caller provides:
*   A valid map struct
* We return:
*   N/A
* We guarantee:
*   All memory associated with the map is freed
*/
void mapDelete(map_t* map);

/**************** loadGold ****************/
/*
* Gold piles are randomly placed around the map
*
* Caller provides:
*   A valid map object
* We return:
*   N/A
* We guarantee:
*   The master map is updated to add gold in random spots
*/
void loadGold(map_t* map);

/**************** getRandomLocation ****************/
/*
* Finds a random valid location on the map to spawn a player
*
* Caller provides:
*   A current master map that also contains where all the playesr are
* We return:
*   An array of two ints representing an x,y coordinate
* We guarantee:
*   The map is a 2D array of char* that represents the same thing as the map text file
* Caller is responsible for:
*   later freeing the map
*/
int* getRandomLocation(map_t* map);

/**************** convertMap ****************/
/*
* Converts the map into a string that can be sent as a message
*
* Caller provides:
*   Takes in a valid map
* We return:
*   A pointer to a string representation of the map
* We guarantee:
*   The map is converted to a string with newlines in the correct place so it can be printed out correctly,
*    also for a player their playerID is replaced with an '@'
* Caller is responsible for:
*   later freeing the string
*/
char* convertMap(map_t* map, char* playerID);

/**************** getLocation ****************/
/*
 * Gets the content at a specified x,y coordinate
 *
 * Caller provides:
 *   A valid map object and integer coordinates for row and column.
 * We return:
 *   A pointer to the string at the specified location, or NULL if the coordinates are out of bounds.
 * We guarantee:
 *   The function will check if the point is in the map
 * Caller is responsible for:
 *   Providing valid coordinates within the map bounds.
 */
char* getLocation(map_t* map, int x, int y);

/**************** setLocation ****************/
/*
 * Sets a specified location in the map to a new value.
 *
 * Caller provides:
 *   A valid map object, integer coordinates for row and column, and a string for the new tile.
 * We return:
 *   None.
 * We guarantee:
 *   The location will be updated if within bounds, otherwise an error is printed.
 * Caller is responsible for:
 *   Ensuring the newTile string remains valid as long as it is in use by the map.
 */
void setLocation(map_t* map, int x, int y, const char* newTile);

/**************** copyMap ****************/
/*
* Takes the master map and produces a map of the same size with all empty spots
*
* Caller provides:
*   A valid map 
* We return:
*   A pointer to a new map with every spot as an empty string
* We guarantee:
*   That the new map is the same dimensions
* Caller is responsible for:
*   Freeing the new copied map
*/
map_t* copyMap(map_t* original);

/**************** replaceGoldSpots ****************/
/*
* Replaces all the gold spots with empty room spots
*
* Caller provides:
*   A valid map 
* We return:
*   N/A
* We guarantee:
*   That map will have all gold spots replaced with empty room spots
*/
void replaceGoldSpots(map_t* map, gameState_t* currState);

#endif // __MAP_H