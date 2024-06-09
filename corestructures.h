/* 
 * corestructures.h - common headfile for common dependent strucutres
 *
 * Implemented according to Implementation Spec
 *
 * TEAM VI, Spring 2024
 */

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "support/hashtable.h"
#include "support/message.h"
#include <sys/types.h>
#include <unistd.h>

/* map_t see implementation.md for details */
typedef struct map {
    char*** mapitem;
    int numRows;
    int numCols;
} map_t;

/* gameState_t see implementation.md for details */
typedef struct gameState {
    map_t* masterMap;
    hashtable_t* player;
    int goldRemaining;
    addr_t spectator;
} gameState_t;

/* player_t see implementation.md for details */
typedef struct player {
    char* playerID; // Player's ID - from A to Z
    char* playerName; // The player's name
    int* playerLocation; // The player's location in the map - x,y coordinates
    int playerGold; // The amount of gold a player has
    int collected;
    map_t* playerVisitedMap; // The visible map for the player
    addr_t playerAddress; // Address to the user of the player
    char* currentSpot; // The value of the player's current location ('#' or '.')
    bool isPlaying; // The player's state

} player_t;

#endif