/* 
 * player.h - header file for player module
 *
 * A *player* is a character for the user to act as in the nuggets game, 
 * and stores the information for the server to use when visualizing the game. 
 * 
 * Team VI, Spring 2024
 */

#ifndef __PLAYER_H
#define __PLAYER_H

#include <stdio.h>
#include <stdbool.h>
#include "../map-module/map.h"
#include "../corestructures.h"

/**************** global types ****************/
// typedef struct player player_t; // Defined in corestructures.h

/**************** functions ****************/

/**************** initializePlayer ****************/
/* Create a new player an assign it to a random location.
 *
 * Caller provides:
 *   The gameState struct.
 *   A player name.
 *   The ip address for the user of the player of type addr_t.
 * We assume:
 *   gameState struct is initialized with all required variables.
 *   playerAddress is a valid ip address for the user.
 * We return:
 *   True if player was initialized succesfully.
 *   False if an error occured.
 */
bool initializePlayer(gameState_t* currState, char* playerName, addr_t playerAddress);

/**************** updateLineOfSight ****************/
/* Compute which parts of the map are visible to the player from their current location. 
 * This function handles the math computations for sightline.
 *
 * Caller provides:
 *   The master map.
 *   The player's current visited map.
 *   The player.
 *   The game State.
 *   A pointer to integers representing the player's location.
 * We return:
 *   true if map was successfully updated.
 *   false if an error occured.
 * We assume:
 *   gameState struct is initialized with all required variables.
 *   player is a valid player.
 */
bool updateLineOfSight(map_t* masterMap, map_t* visibleMap, player_t* player, gameState_t* currstate);

/**************** movePlayer ****************/
/* Update the player's location, their gold, as well as the visible map based on the keystroke given.
 *
 * Caller provides:
 *   The gameState.
 *   A valid player.
 *   A keystroke.
 * We assume:
 *   gameState struct is initialized with all required variables.
 *   player is a valid player.
 */
bool movePlayer(gameState_t* currState, player_t* player, char* keystroke);

/**************** playerQuit ****************/
/* Handle the case when a client(only player) decides to quit.
 *
 * Caller provides:
 *   The gameState.
 *   A valid player.
 * We assume:
 *   gameState struct is initialized with all required variables.
 *   player is a valid player.
 */
void playerQuit(player_t* player);

/**************** playerDelete ****************/
/* Delete a player and free all his information.
 *
 * Caller provides:
 *   A valid player
 * 
 * We assume:
 *   gameState struct is initialized with all required variables.
 *   player is a valid player.
 */
void playerDelete(void* item);

/**************** playerPrint ****************/
/* Print out a player information.
 * Used for testing.
 *
 * Caller provides:
 *   A valid player
 * 
 * We assume:
 *   player is a valid player.
 */
void playerPrint(player_t* player);

#endif // __PLAYER_H