/*
* CS50 Nuggets
* Map Module  File
* Team VI, Spring, 2024
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 
#include <ctype.h>
#include "file.h"
#include "../map-module/map.h"
#include "../corestructures.h"
#include "../player-module/player.h"
#include "../support/hashtable.h"

// Global constants
static const int GoldTotal = 250;      // amount of gold in the game
static const int GoldMinNumPiles = 10; // minimum number of gold piles
static const int GoldMaxNumPiles = 30; // maximum number of gold piles
static const char* EmptySpace = " ";

/*
 * mydup - replacement for the system function strdup
 * From Professor Palmer
*/
char* 
mydup(const char* s)
{
  // make sure s isn't NULL
  if (s == NULL) {
    return NULL;
  }
  else {
    char *p = (char* ) malloc(strlen(s) + 1);
    if (p==NULL) {
      return NULL;
    }
    else {
      strcpy(p,s);
      return p;
    }
  }
}

// Function to free the allocated memory for the map
void 
mapDelete(map_t* map) 
{
    if (map != NULL) {
        if (map->mapitem != NULL) {
            for (int i = 0; i < map->numRows; i++) {
                if (map->mapitem[i] != NULL) {
                    for (int j = 0; j < map->numCols; j++) {
                        free(map->mapitem[i][j]); // Free each string
                    }
                    free(map->mapitem[i]); // Free the array of string pointers
                }
            }
            free(map->mapitem); // Free the top-level pointer array
        }
        free(map); // Free the map structure itself
    }
}

// Function to load the map from a txt file into a map_t structure, initializing the map
/**************** Returns a map object that must be freed after using ****************/
map_t* 
loadMap(const char* filename) 
{
    // making sure the file can be opened
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Error opening file\n");
        return NULL;
    }

    // count number of lines to find number of rows of the map
    int numRows = file_numLines(file);
    rewind(file); // Reset file pointer after counting lines

    // grab the first line to find the number of columns
    char* line = file_readLine(file);
    if (line == NULL || strlen(line) == 0) {
        fprintf(stderr, "Error: Could not read map\n");
        fclose(file);
        return NULL;
    }
    int numCols = strlen(line);

    // allocate memory for the map
    map_t* map = malloc(sizeof(map_t));
    if (map == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for map\n");
        free(line);
        fclose(file);
        return NULL;
    }

    // initialize numRows and numCols for the map
    map->numRows = numRows;
    map->numCols = numCols;

    // allocate memory for the 2D array in the map
    map->mapitem = malloc(numRows * sizeof(char**));

    if (map->mapitem == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for map rows");
        free(map);
        free(line);
        fclose(file);
        return NULL;
    }

    // allocating memory for each array of strings in the map
    for (int i = 0; i < numRows; i++) {
        map->mapitem[i] = malloc(numCols * sizeof(char*));
        if (map->mapitem[i] == NULL) {
            fprintf(stderr, "Error: Failed to allocate memory for map items");
            mapDelete(map);
            free(line);
            fclose(file);
            return NULL;
        }

        // allocating memory for each string that is in the map
        for (int j = 0; j < numCols; j++) {
            map->mapitem[i][j] = malloc(8 * sizeof(char)); // Space for char or numbers and null terminator
            if (map->mapitem[i][j] == NULL) {
                fprintf(stderr, "Error: Failed to allocate memory for map spot");
                mapDelete(map);
                free(line);
                fclose(file);
                return NULL;
            }
            // copy over each char from the map text file into the map 2D array
            if (j < strlen(line)) {
                sprintf(map->mapitem[i][j], "%c", line[j]); 
            } else {
                sprintf(map->mapitem[i][j], " ");  // Filling with spaces if line is shorter than expected
            }
        }
        free(line);                 // free current line
        line = file_readLine(file); // grab next line
        if (line == NULL || strlen(line) == 0) {  // check for empty line or end of file
            break;
        }
    }

    fclose(file);
    return map;
}

// Function that loads gold into the map randomly
void 
loadGold(map_t* map) 
{
    if (map == NULL) {
        fprintf(stderr, "Error: NULL map provided\n");
        return;
    }

    int remainingGold = GoldTotal;
    int numPiles = rand() % (GoldMaxNumPiles - GoldMinNumPiles + 1) + GoldMinNumPiles;
    int successfulPiles = 0;  // to keep track of successful gold placements

    // keeping adding random gold piles until we are out of gold or exeed the allowed number of piles
    while (successfulPiles < numPiles && remainingGold > 0) {
        int i = rand() % map->numRows;  // getting random x location
        int j = rand() % map->numCols;  // getting random y locatoin
        // Ensure the spot is empty and not already a gold pile
        if (map->mapitem[i][j][0] == '.') {
            int goldInPile;
            if (remainingGold < (numPiles - successfulPiles)) {
                goldInPile = remainingGold;
            } else {
                goldInPile = rand() % (remainingGold / (numPiles - successfulPiles)) + 1;
            }
            sprintf(map->mapitem[i][j], "%d", goldInPile);
            remainingGold -= goldInPile;
            successfulPiles++;
        }
    }

    // if there's still gold left after the loop, place it in a random empty spot
    if (remainingGold > 0) {
        int placed = 0;
        while (!placed) {
            int i = rand() % map->numRows;
            int j = rand() % map->numCols;
            if (map->mapitem[i][j][0] == '.') {
                sprintf(map->mapitem[i][j], "%d", remainingGold);
                placed = 1;
            }
        }
    }
}

// Function to get a random spawn location on the map
int* 
getRandomLocation(map_t* map) 
{
    // validating parameter
    if (map == NULL) {
        fprintf(stderr, "Error: NULL map provided\n");
        return NULL;
    }

    int* location = malloc(sizeof(int) * 2); // array to hold the location
    int validLocations[map->numRows * map->numCols][2]; // Array to store valid locations
    int count = 0;

    // Gather all valid locations
    for (int i = 0; i < map->numRows; i++) {
        for (int j = 0; j < map->numCols; j++) {
            if (map->mapitem[i][j][0] == '.') { // Check for empty space
                validLocations[count][0] = i;
                validLocations[count][1] = j;
                count++;
            }
        }
    }

    if (count == 0) {
        return NULL; // No valid location found
    }

    // Choose a random valid location
    int choice = rand() % count;
    location[1] = validLocations[choice][0]; // making suring the x coordinate comes first in the location array
    location[0] = validLocations[choice][1];
    return location;
}

// converts the map into a string that can be sent in messages
/**************** Returns a string that must be freed after using ****************/
char* 
convertMap(map_t* map, char* playerID) 
{
    // validating parameter
    if (map == NULL) {
        fprintf(stderr, "Error: NULL map provided\n");
        return NULL;
    }

    // calculating the size needed for the entire map as a single string including space for newlines and a null terminator
    int size = (map->numRows * (map->numCols + 1)) + 1;
    char* mapString = malloc(size);

    if (mapString == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for map string\n");
        return NULL;
    }

    // looping through the map
    int pos = 0;
    for (int i = 0; i < map->numRows; i++) {
        for (int j = 0; j < map->numCols; j++) {
            // if it sees a string representing a pile a gold switched it to be just a star
            if (isdigit(map->mapitem[i][j][0])) {
                mapString[pos++] = '*';
            }
            else if (isalpha(map->mapitem[i][j][0])) { // replacing a player with @ for their own map
                if (playerID != NULL) { // when priting master map there is no need to replace playerID with @
                    if (strcmp(map->mapitem[i][j], playerID) == 0) {
                        mapString[pos++] = '@';
                    }
                    else {
                        mapString[pos++] = map->mapitem[i][j][0];
                    }
                }
                else {
                    mapString[pos++] = map->mapitem[i][j][0];
                    }
                }
            else {
                mapString[pos++] = map->mapitem[i][j][0];  // directly copy the character
            }
        }
        mapString[pos++] = '\n';  // add a newline after each row
    }
    mapString[pos] = '\0';  // null-terminate the string

    return mapString;
}

// Function to get the content at a specific location in the map
char* 
getLocation(map_t* map, int x, int y) 
{
    if (map == NULL) {
        fprintf(stderr, "Error: NULL map provided\n");
        return NULL;
    }
    
    // Check if the coordinates are within the map bounds
    if (y >= 0 && y < map->numRows && x >= 0 && y < map->numCols) {
        char* spot = malloc(sizeof(char) * 8);
        strcpy(spot, map->mapitem[y][x]);
        return spot;  // Return the string at the specified location
    } else {
        fprintf(stderr, "Error: Coordinates (%d, %d) out of bounds\n", x, y);
        return NULL;  // Return NULL if coordinates are out of bounds
    }
}

// Function to set a location on the map to a certain string 
void 
setLocation(map_t* map, int x, int y, const char* newTile) 
{
    if (map == NULL) {
        fprintf(stderr, "Error: NULL map provided\n");
        return;
    }

    // checking if the coordinates are within the map bounds
    if (y >= 0 && y < map->numRows && x >= 0 && x < map->numCols) {
        
        free(map->mapitem[y][x]); // freeing the old string
        
        // allocating memory for the new string and copying it in
        map->mapitem[y][x] = malloc(strlen(newTile) + 1);
        if (map->mapitem[y][x] == NULL) {
            fprintf(stderr, "Error: Failed to allocate memory for new map spot\n");
            return;
        }
        strcpy(map->mapitem[y][x], newTile);
    } else {
        fprintf(stderr, "Error: Coordinates (%d, %d) out of bounds\n", x, y);
    }
}

// Function that makes a copy of a map with the same dimensions but all blank spaces
map_t* 
copyMap(map_t* original) 
{
    if (original == NULL) {
        fprintf(stderr, "Error: NULL map\n");
        return NULL;
    }

    // allocating memory for the new map 
    map_t* newMap = malloc(sizeof(map_t));
    if (newMap == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for new map\n");
        return NULL;
    }

    // copying the dimension of the original map
    newMap->numRows = original->numRows;
    newMap->numCols = original->numCols;

    // allocating memory for the 2D array in the map
    newMap->mapitem = malloc(newMap->numRows * sizeof(char**));

    if (newMap->mapitem == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory 2D array\n");
        free(newMap); 
        return NULL;
    }

    // allocating memory for each array of strings in the map
    for (int i = 0; i < newMap->numRows; i++) {
        newMap->mapitem[i] = malloc(newMap->numCols * sizeof(char*));
        if (newMap->mapitem[i] == NULL) {
            fprintf(stderr, "Error: Failed to allocate memory for map items\n");
            mapDelete(newMap);
            return NULL;
        }

        // making each spot an empty spot
        for (int j = 0; j < newMap->numCols; j++) {
            newMap->mapitem[i][j] = malloc(8 * sizeof(char));
            if (newMap->mapitem[i][j] == NULL) {
                fprintf(stderr, "Error: Failed to allocate memory for map spot\n");
                mapDelete(newMap);
                return NULL;
            }
            // copying the string in each spot
            strcpy(newMap->mapitem[i][j], EmptySpace);  
        }
    }
    return newMap;
}

/* ***************** playerFind ********************** */
/*
 * Three parameters:
 *      The current game state.
 *      A key to a player in the hashtable.
 *      An item in the hashtable (a player).
 * Helper function find a player in the player hashtable from his ID.
 * To be used with hashtable_iterate function.
 * 
 */
static void 
playerFind(void* arg, const char* key, void* item) {
    player_t* player = item; // Initialize the player    
    player_t** newPlayer = arg;
    if (!(strcmp((*newPlayer)->playerID, player->playerID))) { // Find player
        free((*newPlayer)->playerID);
        free(*newPlayer);
        *newPlayer = player;
    }
}

/* ***************** emptyPlayer ********************** */
/*
 * One parameter:
 *      A player ID.
 * Helper function find a player in the player hashtable from his ID.
 * Initialize a player with the ID passed. This will later be deleted.
 * To be used with hashtable_iterate function.
 * 
 */
static player_t*
emptyPlayer(char* playerID) {
    player_t* newPlayer = malloc(sizeof(player_t));

    newPlayer->playerID = playerID;

    return newPlayer;
}

// Function that replaces all the gold spots with empty location spots
void 
replaceGoldSpots(map_t* map, gameState_t* currState) 
{
    if (map == NULL) {
        fprintf(stderr, "Error: NULL map provided\n");
        return;
    }

    // looping through each spot of the map
    for (int i = 0; i < map->numRows; i++) {
        for (int j = 0; j < map->numCols; j++) {
            // checking if it is a '*'
            if (isdigit(map->mapitem[i][j][0])) {
                // free old string
                free(map->mapitem[i][j]); 
                
                // allocating memory for new string
                map->mapitem[i][j] = malloc(8 * sizeof(char));
                if (map->mapitem[i][j] == NULL) {
                    fprintf(stderr, "Error: Failed to allocate memory for map spot\n");
                    return;
                }
                strcpy(map->mapitem[i][j], ".");  // replace the spot with '.'
            }
            if (isalpha(map->mapitem[i][j][0])) { // replacing players with the old character they were stepping on
                // free old string
                char* IDCopy = mydup(map->mapitem[i][j]);
                free(map->mapitem[i][j]); 

                // finding the player with the corresponding playerID
                player_t* currPlayer = emptyPlayer(IDCopy);
                hashtable_iterate(currState->player, &currPlayer, playerFind);

                // allocating memory for new string
                map->mapitem[i][j] = malloc(8 * sizeof(char));
                if (strcmp(currPlayer->currentSpot, ".") == 0) {
                    strcpy(map->mapitem[i][j], ".");  // replace the spot with '.'
                } else {
                    strcpy(map->mapitem[i][j], "#");  // replace the spot with '#'
                }
            }
        }
    }
}
