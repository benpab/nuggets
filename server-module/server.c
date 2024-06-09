/* 
 * server.c - implements the server and logic for server connection
 *
 * Implemented according to Implementation Spec
 *
 * TEAM VI, Spring 2024
 */

#include "hashtable.h"
#include "../corestructures.h"
#include "map.h"
#include "player.h"
#include "log.h"
#include "message.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <strings.h>
#include <ctype.h> // allows us to check isalpha
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

bool isGameOver = false;
// /**************** Global Constants ****************/
static const int GoldTotalSize = 250;      // amount of gold in the game
static const int MaxPlayerSize = 26;      // maximum number of players
/**************** local functions ****************/
static int parseArgs(const int argc, char* argv[]);
static gameState_t* initializeGameState(int gold, char* mapfile);
static bool handleMessage(void* game, const addr_t from, const char* message);
static void handleSpectator(gameState_t* gameState, char keystroke);
static void updateWorld(gameState_t* gameState);
static void gameOver(gameState_t* gameState);
static void handleSpectator(gameState_t* gameState, char keystroke);
static bool haveSpectator(gameState_t* gameState);
static void removeSpectator(gameState_t* gameState);
static void gameStateDelete(gameState_t* game);
static bool initializeMessage();
static void updatePlayerWorld(void* arg, const char* key, void* item);
static void sendTable(void* arg, const char* key, void* item);
static void makeTable(void* arg, const char* key, void* item);
static void handlePlayMessage(gameState_t* gameState, const addr_t from, const char* content);
static void handleSpectateMessage(gameState_t* gameState, const addr_t from);
static void handleKeyMessage(gameState_t* gameState, const addr_t from, const char* content); 
static void sendInitialGame(gameState_t* gameState, const addr_t from);
static void sendGrid(gameState_t* gameState, const addr_t from);
static void sendGold(gameState_t* gameState, const addr_t from, int collected, int purse);
static void sendDisplay(map_t* map, const addr_t from, char* playerID);
static void sendOK(player_t* player, const addr_t from);
static void sendGameOverMessage(char* table, const addr_t to);

/**************** Functions ****************/
int 
main(const int argc, char* argv[]) 
{
    parseArgs(argc, argv);
    gameState_t* game = initializeGameState(GoldTotalSize, argv[1]);
    loadGold(game->masterMap);
    if (!initializeMessage()) {
        return 2;
    }
    bool ok = message_loop(game, 0, NULL, NULL, handleMessage);
    message_done();
    if (ok) {
        return 0;
    }
    else {
        return 1;
    }
}

/**************** initializeMessage ****************/
/* Sets up the message Module and prints server port number
 * code from support/miniserver.c credit David Kotz - June 2021
 */
static bool
initializeMessage() 
{
    // initialize the message module (without logging)
    int myPort = message_init(NULL);
    if (myPort == 0) {
    return false; // failure to initialize message module
    } else {
    printf("serverPort=%d\n", myPort);
    }
    return true;
} 

/**************** parseArgs ****************/
/* This function parses the commandline arguments and ensures they are valid
* If given a seed number it calls srand with it
* if not, it calls srand with getpid() per requirements
* 
* we exit and return an error message if:
*   not given 2 or 3 arguments
*   the map file is not readable
*   the seed given is not a positive integer
*/
static int 
parseArgs(const int argc, char* argv[]) 
{
    if (argc != 2 && argc != 3) {
        fprintf(stdout, "Args given: %d\n", argc);
        fprintf(stderr, "Error not given correct number of arguments\n");
        exit(3);
    }
    if (argv[1] != NULL) {
        FILE* fp;
        if ((fp = fopen(argv[1], "r")) != NULL) {
            fclose(fp);
        }
        else {
            fprintf(stderr, "Error, not given a readable map file\n");
            exit(4);
        }
    }
    if (argc == 3) {
        // we got a seed so call srand with it
        int seed;
        int scanned;
        if ((scanned = sscanf(argv[2], "%d", &seed)) != 1) {
            fprintf(stderr, "Error, not given an integer seed\n");
            exit(3);
        }
        if (seed < 1) {
            fprintf(stderr, "Error, not given a positive seed\n");
            exit(6);
        }
        srand(seed);
    }
    else {
        // we didn't get a seed
        srand(getpid());
    }
    return 0;
}

/**************** initalizeGameState ****************/
/* This functions initializes the current state of the game 
* by allocating memory for the masterMap and hashtable of players.
*
* Caller provides:
*   the amount of total gold placed, a random int, and the master map
* We return:
*   a gameState ptr to store these values
* We guarantee:
*   gameState struct is initialized with all required variables.
* Otherwise:
*   we print an error to stderr and exit if trouble allocating space
* the caller is responsible for:
*   calling gameStateDelete when done to free game and players
*/
static gameState_t* 
initializeGameState(int gold, char* mapfile)
{
    gameState_t* game = malloc(sizeof(gameState_t));
    if (game == NULL) {
        fprintf(stderr, "Error allocating for gameState\n");
        exit(5);
    }
    hashtable_t* players = hashtable_new(MaxPlayerSize);
    if (players == NULL) {
        fprintf(stderr, "Error allocating players\n");
        exit(5);
    }
    game->masterMap = loadMap(mapfile);
    game->goldRemaining = gold;
    game->spectator = message_noAddr();
    game->player = players;
    return game;
}

/**************** gameStateDelete ****************/
/* frees all the necessary data structures in a gameState
* 
* These data structures being the hashtable of players and the mastermap
*  Caller provides:
*   A pointer to a gameState_t
*  We guarentee:
*    All the space used by a gameState is free afterwards
*/
static void
gameStateDelete(gameState_t* gameState)
{
    if (gameState != NULL) {
        if (gameState->player != NULL) {
            hashtable_delete(gameState->player, playerDelete); 
        }
        if (gameState->masterMap != NULL) {
            mapDelete(gameState->masterMap);
        }
        free(gameState);
    }
}

/**************** handleMessage ****************/
/* This function handles logic based on messages received
* 
* Specifically: That is, the function determines if a new player should be added
* or if a key input needs to be handled. 
*
* This function also returns the global gameRunning boolean to decide when to end.
*
*/
static bool
handleMessage(void* game, const addr_t from, const char* message)
{
    gameState_t* gameState = (gameState_t*) game;
    if (strncmp(message, "PLAY ", strlen("PLAY ")) == 0) {
        const char* content = message + strlen("PLAY ");
        handlePlayMessage(gameState, from, content);
    }
    else if (strcmp(message, "SPECTATE") == 0) {
        handleSpectateMessage(gameState, from);
    }
    else if (strncmp(message, "KEY ", strlen("KEY ")) == 0) {
        const char* content = message + strlen("KEY ");
        handleKeyMessage(gameState, from, content);
    }
    else {
        fprintf(stderr, "Received an unexpected message: %s\n this message was received from %s", message, message_stringAddr(from));
    }
    return isGameOver;
    // we return true when the game is over
}

/**************** handlePlayMessage ****************/
/* This function decides how to respond to a PLAY <name> message 
* 
* If we are unable to initialize a player, due to invalid name or memory error
* we send a message in initializePlayer
*/
static void
handlePlayMessage(gameState_t* gameState, const addr_t from, const char* content) 
{
    if (hashtable_find(gameState->player, message_stringAddr(from))) {
        fprintf(stderr, "Player is trying to rejoin: %s\n", message_stringAddr(from));
    }
    else if (initializePlayer(gameState, (char*) content, from)) {
        sendOK(hashtable_find(gameState->player, message_stringAddr(from)), from);
        sendInitialGame(gameState, from);
    }
}

/**************** sendInitialGame ****************/
/* This function sends the initial game info to a player
* This includes GRID, GOLD, and DISPLAY messages as defined in REQUIREMENTS
*/
static void 
sendInitialGame(gameState_t* gameState, const addr_t from)
{
    player_t* player = hashtable_find(gameState->player, message_stringAddr(from));
    sendGrid(gameState, from);
    sendGold(gameState, from, 0, 0);
    sendDisplay(player->playerVisitedMap, from, player->playerID);
}

/**************** sendOK ****************/
/* This function sends the OK message to a client
* This includes OK <playerID>
*/
static void 
sendOK(player_t* player, const addr_t from)
{
    char msg[strlen("OK C")];
    sprintf(msg, "OK %s", player->playerID);
    message_send(from, msg);
}


/**************** sendGrid ****************/
/* This function sends the GRID info to a client
* This includes numRows and numCols
* LIMITATION: we only allow for sending a grid of up to 6 digits by 6 digits
*             This accounts for the largest known playable map
*/
static void
sendGrid(gameState_t* gameState, const addr_t from) 
{
    char msg[strlen("GRID numRow numCol ")];
    sprintf(msg, "GRID %d %d", gameState->masterMap->numRows, gameState->masterMap->numCols);
    message_send(from, msg);
}

/**************** sendGold ****************/
/* This function sends the GOLD info to a client
* This includes collected, amount in the purse, and remaining gold
* LIMITATION: we only account for 12 digits of gold, this is far more than our game has
*/
static void 
sendGold(gameState_t* gameState, const addr_t from, int collected, int purse)
{
    char msg[strlen("GOLD ngold pgold rgold ")];
    sprintf(msg, "GOLD %d %d %d", collected, purse, gameState->goldRemaining);
    message_send(from, msg);
}

/**************** sendDisplay ****************/
/* This function sends the DISPLAY info to a client
* This converts the map to a string representation, then adds DISPLAY\n to the front
*/
static void
sendDisplay(map_t* map, const addr_t from, char* playerID) 
{
    char* stringMap = convertMap(map, playerID);
    char displayStr[strlen(stringMap) + strlen("DISPLAY\n") + 1];
    sprintf(displayStr, "DISPLAY\n%s", stringMap);
    message_send(from, displayStr);
    free(stringMap);
}

/**************** handleSpectateMessage ****************/
/* This function decides how to respond to a SPECTATE message 
* 
* If a spectator is already in the game, we send them a message, 
* remove them, then add a new spectator and send them gameInfo
*/
static void
handleSpectateMessage(gameState_t* gameState, const addr_t from)
{
    // if we have a current spectator
    if (haveSpectator(gameState)) {
        // remove them
        removeSpectator(gameState);
    }   
    // set the spectator to be whoever is asking
    gameState->spectator = from;
    sendGrid(gameState, from);
    sendGold(gameState, from, 0, 0);
    sendDisplay(gameState->masterMap, from, NULL);
}

/**************** handleKeyMessage ****************/
/* This function handles keystrokes 
* 
* Decides logic based on if it was received from a spectator or player
* If a player sends a keystroke we call move player with the key
* If a non-player and non-spectator call KEY <key> we log it and do nothing
*
* NOTE: This function implements sprinting ability.
*       It does so by continously calling movePlayer as long as movePlayer returns true
*       and the keystroke we are given is uppercase. This was the most logical way we could
*       implement this.
*/
static void 
handleKeyMessage(gameState_t* gameState, const addr_t from, const char* content)
{
    // if the message we receive is from a spectator
    if (message_eqAddr(from, gameState->spectator)) {
        handleSpectator(gameState, content[0]);
    }
    // otherwise we check if it is a player
    else { 
        player_t* curPlayer = (player_t*) hashtable_find(gameState->player, message_stringAddr(from));
        if (curPlayer != NULL) { // a player is sending a keystroke
            // while loop for sprinting ability
            while (movePlayer(gameState, curPlayer, (char*) content) && isupper(content[0])) {
            }
            updateWorld(gameState);
        }
        // we do not send a message if they are not a player or spectator
        // instead just log what we received
        else {
            fprintf(stderr, "Received a keystroke from a non-spectator and non-player. Keystroke: %s", content);
        }
    }
}

/**************** handleSpectator ****************/
/* This function handles keystrokes from a spectator. 
* The only keystroke we envision handling is 'q' to quit, we ignore all others.
* This function is also used to force a spectator to quit when another one joins
* If we receive an unexpected keystroke we log the keystroke with a message and do nothing
*/
static void 
handleSpectator(gameState_t* gameState, char keystroke)
{
    if (keystroke == 'q') {
        message_send(gameState->spectator, "QUIT Thanks for watching!");
        gameState->spectator = message_noAddr();
    }
    else {
        fprintf(stderr, "Received an unexpected keystroke from spectator, Keystroke: %c", keystroke);
    }
}

/**************** removeSpectator ****************/
/* This function removes a spectator when we will replace them */
static void 
removeSpectator(gameState_t* gameState) 
{
    message_send(gameState->spectator, "QUIT You have been replaced by a new spectator.");
    gameState->spectator = message_noAddr();
}

/**************** updateWorld ****************/
/* This function handles updating the world state for each player
* We update the screen for each player who is playing
* This function loops through all players and sends them a new gamestate
* If there is a spectator we also send the new game to the spectator
* If the game is over we call gameOver and set the global variable to true
* We also print out the masterMap here for the server as David Kotz did in his
* example video
*/
static void 
updateWorld(gameState_t* gameState)
{
    if (gameState->goldRemaining == 0) {
        isGameOver = true;
        gameOver(gameState);
    }
    else {
        hashtable_iterate(gameState->player, gameState, updatePlayerWorld);
        if (haveSpectator(gameState)) {
            sendGold(gameState, gameState->spectator, 0, 0);
            sendDisplay(gameState->masterMap, gameState->spectator, NULL);
        }
    }
}

/**************** updatePlayerWorld ****************/
/* updates the world for each individual player
* this is a helper function for hashtable_iterate
* This only updates their world if they are still playing
*/
static void 
updatePlayerWorld(void* arg, const char* key, void* item)
{
    // reconstruct our datatypes
    gameState_t* gameState = (gameState_t*) arg;
    player_t* player = (player_t*) item;
    // only send an updated world if they are playing
    if (player->isPlaying) {
        addr_t player_addr = player->playerAddress;
        sendGold(gameState, player_addr, player->collected, player->playerGold);
        sendDisplay(player->playerVisitedMap, player_addr, player->playerID);
    }
}

/**************** updateWorld ****************/
/* a simple wrapper that checks if we have a spectator with message_isAddr*/
static bool
haveSpectator(gameState_t* gameState) 
{
    return (message_isAddr(gameState->spectator));
}

/**************** gameOver ****************/
/* This function creates and sends the end game state
* this includes creating the table, then sending it
* once we have sent the final table, clean up the table and gameState
*/
static void 
gameOver(gameState_t* gameState) 
{
    // create a table to pass to hashtable_iterate
    char* table = malloc(sizeof(char));
    if (table == NULL) {
        fprintf(stderr, "Error allocating initial table\n");
        exit(5);
    }
    strcpy(table, "");
    // create the table
    hashtable_iterate(gameState->player, &table, makeTable);

    // send the table
    hashtable_iterate(gameState->player, table, sendTable);
    // send the table to spectator
    if (haveSpectator(gameState)) {
        sendGameOverMessage(table, gameState->spectator);
    }

    // free the table
    free(table);
    gameStateDelete(gameState);
}

/**************** sendGameOverMessage ****************/
/* This function sends a game over message and the final table to the given address
*/
static void
sendGameOverMessage(char* table, const addr_t to)
{
    char message[strlen(table) + 1 + strlen("QUIT GAME OVER:\n")];
    sprintf(message, "QUIT GAME OVER:\n%s", table);
    message_send(to, message);
}

/**************** makeTable ****************/
/* This function adds the current players stats to a table
* this is a helper function to hashtable_iterate
* The measurements and sizes were based on examples from REQUIREMENT
*/
static void 
makeTable(void* arg, const char* key, void* item)
{
    // reconstruct table and player
    char** table = (char**) arg;
    player_t* player = (player_t*) item;

    // buffer for formatting table
    int goldbuf = 10;
    // size is based on "PLAYERID GOLD PLAYERNAME" ex "A          4 Alice\n"
    int newEntrySize = strlen(player->playerID) + goldbuf + 1 + strlen(player->playerName) + 1 + 1;
    size_t newsize = sizeof(char) * (strlen(*table) + newEntrySize + 1);
    char* newTable = realloc(*table, newsize);
    // ensure we had space to alloc
    if (newTable == NULL) {
        fprintf(stderr, "Error allocating memory for final table\n");
        exit(5);
    }
    // first put the line in a temp char*
    char line[newEntrySize + 1];
    snprintf(line, newEntrySize + 1,  "%s%10d %s\n", player->playerID, player->playerGold, player->playerName);
    // then copy it over
    strcat(newTable, line);
    *table = newTable;
}

/**************** sendTable ****************/
/* This function sends the completed table to all the players
* this is a helper function to hashtable_iterate
*/
static void 
sendTable(void* arg, const char* key, void* item)
{
    // reconstruct table and player
    char* table = (char*) arg;
    player_t* player = (player_t*) item;
    sendGameOverMessage(table, player->playerAddress);
}
