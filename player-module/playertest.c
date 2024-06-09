/* 
 * playertest.c - implements the server and logic for server connection and intergrates with player module.
 *
 * Implemented according to Implementation Spec
 * 
 * Builds on server.c file
 *
 * TEAM VI, Spring 2024
 */

#include "hashtable.h"
#include "../corestructures.h"
#include "../map-module/map.h"
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

/**************** local functions ****************/
static int parseArgs(const int argc, char* argv[]);
static gameState_t* initializeGameState(int gold, char* mapfile);
static void gameStateDelete(gameState_t* gameState);

// Constants
const char* FreeSpot = ".";

int 
main(const int argc, char* argv[]) 
{
    parseArgs(argc, argv);

    gameState_t* game = initializeGameState(100, argv[1]);

    char* mapStr = convertMap(game->masterMap, NULL);
    printf("Master map:\n");
    printf("%s\n", mapStr);
    free(mapStr);

    // Add player
    addr_t addr1 = message_noAddr();
    addr1.sin_port += 1;

    // Initialize Player with no player name
    printf("Initialize player with invalid name\n");
    if (!initializePlayer(game, "   ", addr1)) {
        printf("Could not initialize player\n");
    }
    printf("\n");

    printf("Initialize valid player:\n\n");
    if (!initializePlayer(game, "John", addr1)) {
        fprintf(stderr, "Error: Could not initialize player");
        exit(1);
    }
    
    // Print out player information
    player_t* player1 = hashtable_find(game->player, message_stringAddr(addr1));
    playerPrint(player1);

    mapStr = convertMap(game->masterMap, NULL);
    printf("Master map:\n");
    printf("%s\n", mapStr);
    free(mapStr);

    // Move Player in all directions
    printf("Move Player in all directions:\n\n");

    printf("Left\n");
    movePlayer(game, player1, "h");
    playerPrint(player1);

    printf("Right\n");
    movePlayer(game, player1, "l");
    playerPrint(player1);

    printf("Down\n");
    movePlayer(game, player1, "j");
    playerPrint(player1);

    printf("Up\n");
    movePlayer(game, player1, "k");
    playerPrint(player1);

    printf("Up and left\n");
    movePlayer(game, player1, "y");
    playerPrint(player1);

    printf("Up and right\n");
    movePlayer(game, player1, "u");
    playerPrint(player1);

    printf("Down and left\n");
    movePlayer(game, player1, "b");
    playerPrint(player1);

    printf("Down and right\n");
    movePlayer(game, player1, "n");
    playerPrint(player1);

    // Add a second player
    printf("Add another player:\n\n");
    addr_t addr2 = message_noAddr();
    addr2.sin_port += 2; // Get a new port and add a player


    // Initialize a new player
    if (!initializePlayer(game, "Jane", addr2)) {
        fprintf(stderr, "Error: Could not initialize player");
        exit(1);
    }

    // Set players location next to each other
    player_t* player2 = hashtable_find(game->player, message_stringAddr(addr2));

    printf("Print both players:\n\n");
    playerPrint(player1);
    playerPrint(player2);

    // Move players
    printf("Move both players right:\n\n");
    movePlayer(game, player1, "l"); // Move Right
    movePlayer(game, player2, "l"); // Move Right

    // Print the master map
    mapStr = convertMap(game->masterMap, NULL);
    printf("Master map:\n");
    printf("%s\n", mapStr);
    free(mapStr);

    // Make players crash into each other
    printf("Test players crashing into each other:\n\n");

    // Change first player's location
    setLocation(game->masterMap, 4, 2, player1->playerID);
    setLocation(game->masterMap, player1->playerLocation[0], player1->playerLocation[1], player1->currentSpot); // Change old location
    player1->playerLocation[0] = 4;
    player1->playerLocation[1] = 2;
    strcpy(player1->currentSpot, ".");

    // Change second player's location
    setLocation(game->masterMap, 5, 2, player2->playerID);
    setLocation(game->masterMap, player2->playerLocation[0], player2->playerLocation[1], player2->currentSpot); // Change old location
    player2->playerLocation[0] = 5;
    player2->playerLocation[1] = 2;
    strcpy(player2->currentSpot, ".");

    // Print the master map
    mapStr = convertMap(game->masterMap, NULL);
    printf("Master map (Players should be next to each other):\n");
    printf("%s\n", mapStr);
    free(mapStr);

    printf("Make player's crash:\n");
    movePlayer(game, player1, "l"); // Move player 1 right (into player 2)

    // Print the master map
    mapStr = convertMap(game->masterMap, NULL);
    printf("Master map (Player's should have swapped locations):\n");
    printf("%s\n", mapStr);
    free(mapStr);

    // Quit player
    printf("Quit players\n");
    movePlayer(game, player1, "Q");
    movePlayer(game, player2, "Q");
    printf("\n");
    playerPrint(player1);    
    playerPrint(player2); 

    // Delete Game state
    printf("End game.\n");
    gameStateDelete(game);
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
            exit(5);
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
        exit(1);
    }
    hashtable_t* players = hashtable_new(26);
    if (players == NULL) {
        fprintf(stderr, "Error allocating players\n");
        exit(1);
    }
    game->masterMap = loadMap(mapfile);
    loadGold(game->masterMap);
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