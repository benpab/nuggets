/* 
 * player.c - player module for nuggets game
 *
 * see player.h for more information.
 *
 * Team VI, Spring 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "player.h"
#include "map.h"
#include "../corestructures.h"
#include "hashtable.h"
#include "message.h"

/**************** Global Constants ****************/
static const int MaxNameLength = 50;   // max number of chars in playerName
static const int MaxPlayers = 26;      // maximum number of players
static const int ASCIIStart = 65; // The first ASCII value for letters - 'A'

// Constant for each character
static const char Quit = 'q';
static const char Left = 'h';
static const char Right = 'l';
static const char Up = 'k';
static const char Down = 'j';
static const char UpLeft = 'y';
static const char UpRight = 'u';
static const char DownLeft = 'b';
static const char DownRight = 'n';

// String representations for the map
static const char* EmptySpot = ".";
static const char* Bridge = "#";
static const char* NotInMap = " ";
static const char* CornerWall = "+";
static const char* VerticalWall = "|";
static const char* HorizontalWall = "-";

/**************** functions ****************/

/**************** local functions ****************/
static void getSize(void* arg, const char* key, void* item);
static char* findPlayerID(const int numOfPlayers);
static bool isValidName(const char *str);
static bool move(gameState_t* currState, player_t* player, int newLocX, int newLocY);
static void playerFind(void* arg, const char* key, void* item);
static bool BresenhamLOS(map_t* masterMap, map_t* visibleMap, int x0, int y0, int x1, int y1);
static player_t* emptyPlayer(char* playerID);

/**************** global functions ****************/

/**************** initializePlayer ****************/
/* see player.h for description */
bool 
initializePlayer(gameState_t* currState, char* playerName, addr_t playerAddress) {
    player_t* player = malloc(sizeof(player_t));
    int numPlayers = 0; // Counter for the number of players in the game
    hashtable_t* playerHt = currState->player;
    int* randomLocation;
    map_t* masterMap = currState->masterMap; // The master map
    
    // Iterate over the hashtable to get the number of players
    hashtable_iterate(playerHt, &numPlayers, getSize);

    if (numPlayers < MaxPlayers) {
        if (isValidName(playerName)) {
            char* name = malloc(sizeof(char) * (MaxNameLength + 1));
            strncpy(name, playerName, MaxNameLength);
            name[MaxNameLength] = '\0';
            player->playerName = name;
            player->playerID = findPlayerID(numPlayers); // Get the playerID
            if ((randomLocation = getRandomLocation(masterMap)) != NULL) {
                player->playerLocation = randomLocation; // Find a random location for the player
                player->currentSpot = getLocation(masterMap, player->playerLocation[0], player->playerLocation[1]); // Get the current player spot
                player->playerGold = 0; // Initialize playerGold as 0
                player->collected = 0;
                player->playerVisitedMap = copyMap(masterMap); // Initialize player visited map as empty
                updateLineOfSight(masterMap, player->playerVisitedMap, player, currState); // Find current visible map
                player->playerAddress = playerAddress; // Store the user's address
                player->isPlaying = true; // initialize player as playing
                hashtable_insert(playerHt, message_stringAddr(playerAddress), player); // Add the player to the hashtable with the address as the key
                
                // Update the map
                setLocation(currState->masterMap, player->playerLocation[0], player->playerLocation[1], player->playerID);
                //updateWorld(currState);

                return true;
            }
            else {
                message_send(playerAddress, "QUIT Could not initialize player."); // Error initializing player
                free(name);
                free(player->playerID);
                free(player); // Free the player memory
            }
        } 
        else {
            message_send(playerAddress, "QUIT Sorry - you must provide player's name."); // Send back an error message
            free(player); // Free the player memory
        }
    }
    else {
        message_send(playerAddress, "QUIT Game is full: no more players can join."); // Send back an error message
        free(player); // Free the player memory
    }
    return false;
}

/**************** movePlayer ****************/
/* see player.h for description */
bool 
movePlayer(gameState_t* currState, player_t* player, char* keystroke) {
    char normalizedKeyStroke = tolower(keystroke[0]); // Put each keystroke as lowercase
    if (normalizedKeyStroke == Quit) {
        setLocation(currState->masterMap, player->playerLocation[0], player->playerLocation[1], player->currentSpot); // Update the master map to remove player
        playerQuit(player); // If the player decides to quit, then quit
    }
    else {
        // Get X and Y coordinates of the player
        int locX = player->playerLocation[0];
        int locY = player->playerLocation[1];

        // For each movement find the new location and send to move function
        if (normalizedKeyStroke == Left) {
            return move(currState, player, locX - 1, locY);
        }
        else if (normalizedKeyStroke == Right) {
            return move(currState, player, locX + 1, locY);
        }
        else if (normalizedKeyStroke == Up) {
            return move(currState, player, locX, locY - 1);
        }
        else if (normalizedKeyStroke == Down) {
            return move(currState, player, locX, locY + 1);
        }
        else if (normalizedKeyStroke == UpLeft) {
            return move(currState, player, locX - 1, locY - 1);
        }
        else if (normalizedKeyStroke == UpRight) {
            return move(currState, player, locX + 1, locY - 1);
        }
        else if (normalizedKeyStroke == DownLeft) {
            return move(currState, player, locX - 1, locY + 1);
        }
        else if (normalizedKeyStroke == DownRight) {
            return move(currState, player, locX + 1, locY + 1);
        }
    }
    return false;
}

/**************** updateLineOfSight ****************/
/* see player.h for description */
bool
updateLineOfSight(map_t* masterMap, map_t* visibleMap, player_t* player, gameState_t* currstate)
{

    // Get number of rows and columns
    int nRows = masterMap->numRows;
    int nCols = masterMap->numCols;

    // Get player information
    char* playerID = player->playerID;
    int* playerLocation = player->playerLocation;

    if(masterMap == NULL || visibleMap == NULL){
        fprintf(stderr, "Error: improper parameters passed to computeVisibility\n");
        return false;
    } else if(nRows < 1 || nCols < 1){
        fprintf(stderr, "Error: nRows or nCols less than 1 passed to computeVisibility\n");
        return false;
    }

    // Get player Location
    int x0 = playerLocation[0];
    int y0 = playerLocation[1];

    replaceGoldSpots(visibleMap, currstate); // Get rid of all gold spots

    // Add the playerID to the visible map
    setLocation(visibleMap, x0, y0, playerID);

    bool res1 = false;
    bool res2 = false;
    for(int x1 = 0; x1 < nCols; x1++){ //top and bottom
        res1 = BresenhamLOS(masterMap, visibleMap, x0, y0, x1, 0);
        res2 = BresenhamLOS(masterMap, visibleMap, x0, y0, x1, (nRows-1));
    }
    if(res1==false || res2 == false){
        return false;
    }
    for(int y1 = 0; y1 < nRows; y1++){ //sides
        res1 = BresenhamLOS(masterMap, visibleMap, x0, y0, 0, y1);
        res2 = BresenhamLOS(masterMap, visibleMap, x0, y0, (nCols-1), y1);
    }
    if(res1==false || res2 == false){
        return false;
    }
    return true;
}

/**************** playerQuit ****************/
/* see player.h for description */
void 
playerQuit(player_t* player)
{
    if (player != NULL) {
        player->isPlaying = false;
        message_send(player->playerAddress, "QUIT Thanks for playing!");
    }
}

/**************** playerDelete ****************/
/* see player.h for description */
void 
playerDelete(void* item) {
    player_t* player = item;
    mapDelete(player->playerVisitedMap); // Free the map
    free(player->playerName); // Free the player's name
    free(player->playerID); // Free the player ID
    free(player->playerLocation);
    free(player->currentSpot);
    free(player); // Free the player
}

/**************** playerPrint ****************/
/* see player.h for description */
void playerPrint(player_t* player) {
    // Print out all the player's information
    printf("Player Information\n");
    printf("Player ID: %s\n", player->playerID);
    printf("Player Name: %s\n", player->playerName);
    printf("Player Location: (%d, %d)\n", player->playerLocation[0], player->playerLocation[1]);
    printf("Current Spot: %s\n", player->currentSpot);
    printf("Player Address: %s\n", message_stringAddr(player->playerAddress));
    printf("Active (1 if active, 0 if inactive): %d\n", player->isPlaying);
    printf("Player Gold count: %d\n", player->playerGold);

    char* mapStr = convertMap(player->playerVisitedMap, NULL);
    printf("Player Visited Map:\n%s\n", mapStr);
    free(mapStr);
}


/* ***************** getSize ********************** */
/*
 * Three parameters:
 *      arg: A pointer to an integer.
 *      The current key in the hashtable.
 *      The item for the current key.
 * Helper function to find the size of a hashtable.
 */
static void 
getSize(void* arg, const char* key, void* item) 
{
    // Increment the size by one
    int* size = arg;
    *size += 1;
}

/* ***************** findPlayerId ********************** */
/*
 * One parameter:
 *      An integer for the number of players in the game.
 * Helper function to find the playerID for a player from the size of a player hashtable.
 * 
 * We return:
 *      The player name
 */
static char* findPlayerID(const int numOfPlayers) {
    char* playerID = malloc(2 * sizeof(char*)); // Initialize the playerName
    playerID[0] = (char) (ASCIIStart + numOfPlayers); // Get the character
    playerID[1] = '\0'; // Add the null character

    return playerID;
}

/* ***************** isValidName ********************** */
/*
 * One parameter:
 *      A player's name.
 * Helper function to determine if a player's name is valid.
 * 
 * We return:
 *      True if the name is valid.
 *      False if it is not valid.
 */
static bool 
isValidName(const char *str) {
    if (str == NULL) {
        return false; // The string is not valid if it is NULL
    }

    while (*str != '\0') {
        if (!isspace((unsigned char)*str)) {
            return true; // The string is valid if it contains at least one non-space character
        }
        str++;
    }

    return false; // The string is not valid if all characters are spaces or if it is empty
}

/* ***************** move ********************** */
/*
 * Three parameters:
 *      The current game state.
 *      The player to move.
 *      X and Y coordinates for the new loctation in the map.
 * Helper function to move a player in the map to a new location.
 * 
 * We return:
 *    True if player changed locations.
 *    False otherwise.
 */
static bool 
move(gameState_t* currState, player_t* player, int newLocX, int newLocY) {
    map_t* currmap = currState->masterMap;

    // Get old location
    int oldLocX = player->playerLocation[0];
    int oldLocY = player->playerLocation[1];

    char* newLocation = getLocation(currmap, newLocX, newLocY);

    // Check if the new location is in the map
    if (strcmp(newLocation, NotInMap)) {
        // Check if new location has gold
        if (atoi(newLocation) != 0) {
            setLocation(currmap, oldLocX, oldLocY, player->currentSpot); // Update the old location
            
            player->playerGold += atoi(newLocation); // Update player's gold

            currState->goldRemaining -= atoi(newLocation); // Update remaining gold

            player->collected = atoi(newLocation);
            strcpy(player->currentSpot, EmptySpot); // Update the type for the current spot
        
            // Update current location for the player
            player->playerLocation[0] = newLocX;
            player->playerLocation[1] = newLocY;

            // Update user visibility
            setLocation(currmap, newLocX, newLocY, player->playerID); // Update the master map
            updateLineOfSight(currmap, player->playerVisitedMap, player, currState);

            free(newLocation);
            return true;
        }
        // Check if new location is a bridge
        else if (!strcmp(newLocation, Bridge)) {
            player->collected = 0; // No gold found
            setLocation(currmap, oldLocX, oldLocY, player->currentSpot); // Update the old location
        
            // Update current location for the player
            player->playerLocation[0] = newLocX;
            player->playerLocation[1] = newLocY;
            
            strcpy(player->currentSpot, Bridge);

            // Update user visibility

            setLocation(currmap, newLocX, newLocY, player->playerID); // Update the master map
            updateLineOfSight(currmap, player->playerVisitedMap, player, currState);

            free(newLocation);
            return true;
        }
        // Check if new location is an empty spot
        else if (!strcmp(newLocation, EmptySpot)) {
            player->collected = 0; // No gold found
            setLocation(currmap, oldLocX, oldLocY, player->currentSpot); // Update the old location
        
            // Update current location for the player
            player->playerLocation[0] = newLocX;
            player->playerLocation[1] = newLocY;

            strcpy(player->currentSpot, EmptySpot);

            // Update user visibility
            setLocation(currmap, newLocX, newLocY, player->playerID); // Update the master map
            updateLineOfSight(currmap, player->playerVisitedMap, player, currState);

            free(newLocation);
            return true;
        }
        // Check if new location has a character
        else if (isalpha(newLocation[0])) {
            // Find the player
            player_t* newPlayer = emptyPlayer(newLocation);
            hashtable_iterate(currState->player, &newPlayer, playerFind);
            if (newPlayer != NULL) { // Error prevention, This should never happen
                
                // Store other player's location
                char* newPlayerCurrspot = malloc(strlen(newPlayer->currentSpot)+1);
                strcpy(newPlayerCurrspot, newPlayer->currentSpot);
                
                strcpy(newPlayer->currentSpot, player->currentSpot);
                newPlayer->playerLocation[0] = oldLocX;
                newPlayer->playerLocation[1] = oldLocY;
                newPlayer->collected = 0;
                
                // Update our player's location
                strcpy(player->currentSpot, newPlayerCurrspot);
                player->playerLocation[0] = newLocX;
                player->playerLocation[1] = newLocY;
                player->collected = 0; // No gold found

                setLocation(currmap, oldLocX, oldLocY, newPlayer->playerID);
                setLocation(currmap, newLocX, newLocY, player->playerID);
                

                // Update user visibility
                updateLineOfSight(currmap, player->playerVisitedMap, player, currState);
                updateLineOfSight(currmap, newPlayer->playerVisitedMap, newPlayer, currState);

                free(newPlayerCurrspot);
            }

            // free(newLocation);
            return true;
        }
    }
    free(newLocation);
    return false;
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


/* ***************** playerFind ********************** */
/*
 * Three parameters:
 *      The current map
 *      The map to calculate user visibility
 *      x,y coordinates for player location.
 *      x,y coordinates in the visible map
 * Helper function to compute the user visibility for a player.
 * 
 * We return:
 *  True if success.
 *  False if error occurs.
 */
static bool 
BresenhamLOS(map_t* masterMap, map_t* visibleMap, int x0, int y0, int x1, int y1){
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx - dy;
    char* currentLocation; // Current Location to check
    currentLocation = getLocation(masterMap, x0, y0);
    while (1) {
        if (!strcmp(currentLocation, NotInMap) || !strcmp(currentLocation, CornerWall) || !strcmp(currentLocation, VerticalWall) || !strcmp(currentLocation, HorizontalWall) || !strcmp(currentLocation, Bridge)) {
            free(currentLocation); // Free the current location
            break;
        }
        if (x0 == x1 && y0 == y1) {
            free(currentLocation); // Free the current location
            break;
        }
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y0 += sy;
        }
        free(currentLocation); // Free Current loaction
        currentLocation = getLocation(masterMap, x0, y0);
        setLocation(visibleMap, x0, y0, currentLocation);
    }
    return true;
}