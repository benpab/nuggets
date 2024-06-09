
/* 
 * client.c - implements the client and logic for client connection
 *
 * Implemented according to Implementation Spec
 *
 * TEAM VI, Spring 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../support/message.h"
#include <unistd.h>
#include <ncurses.h>
 

/**************** local functions ****************/
static bool recieveMessage(void* arg, const addr_t from, const char* message);
static bool sendMessage(void* arg);
static void initCurses();
static void initMap();
static void printMap(const char* display);

/**************** helper functions ****************/
static bool handleOK(const char* message);
static bool handleGRID(const char* message);
static bool handleGOLD(const char* message);
static void handleDISPLAY(const char* message);

/**************** global variables ****************/
typedef struct clientInfo { //way to store map size and player icon for printing
    int nRows;
    int nCols;
    char player;
    addr_t serverAddr;
} clientInfo_t;    
static clientInfo_t* clientInfo_new();
static clientInfo_t* clientInfo;

/***************** main *******************************/
int
main(const int argc, char* argv[])
{
    // initialize the message module (without logging)
    if(message_init(NULL) == 0){
        return 2; // failure to initialize message module
    }

    // check arguments  
    const char* program = argv[0];
    if(argc != 3 && argc != 4){
        fprintf(stderr, "usage: %s hostname port <PLAYERNAME>\n", program);
        return 3; // bad commandline
    }
    
    // commandline provides address for server
    const char* serverHost = argv[1];
    const char* serverPort = argv[2];
    addr_t server; // address of the server
    if(!message_setAddr(serverHost, serverPort, &server)){
        fprintf(stderr, "can't form address from %s %s\n", serverHost, serverPort);
        return 4; // bad hostname/port
    }
    clientInfo = clientInfo_new();
    clientInfo->serverAddr = server;
    if(argc == 4){ //initialize playing
        char initMsg[6+strlen(argv[3])];
        strcpy(initMsg, "PLAY ");
        strcat(initMsg, argv[3]);
        message_send(server, initMsg);
    }else{
        message_send(server, "SPECTATE");
    }
    // Loop, waiting for input or for messages; provide callback functions.
    // We use the 'arg' parameter to carry a pointer to 'server'.
    bool ok = message_loop(&server, 0, NULL, sendMessage, recieveMessage);

    endwin(); // end CURSES
    // shut down the message module
    message_done();
    free(clientInfo);

    return ok? 0 : 1; // status code depends on result of message_loop
}

/**************** sendMessage ****************/
/*  Handles input from ncurses in order to send messages to the server
 * always sends messages with the format "KEY <latest key pressed>"
 */
static bool sendMessage(void* arg){
        addr_t* server = (addr_t*)arg;
        if(server == NULL){ //if no parameter passed. Should NEVER happen unless we provided a bogus server
                fprintf(stderr, "sendMessage called without valid server\n");
                return true;
        }
        if(!message_isAddr(*server)){ //should NEVER happen.. unless server perhaps crashes?
                fprintf(stderr, "sendMessage called without valid server\n.");
                return true;
        }
        char c = getch(); //get input from curses
        char message[strlen("KEY ") + 3];
        strcpy(message, "KEY ");
        strcat(message, &c); //add curses input
        message[strlen("KEY ") + 2] = '\0'; //set null char to make sure string terminates
        message_send(*server, message); //send message
        return false;
}


/**************** recieveMessage ****************/
/* Given a message from the server, this function parses it, and decides how it should be handled
 * Malformed messages are logged to stderr and otherwise ignored
 */
static bool recieveMessage(void* arg, const addr_t from, const char* message){ 
    //figure out what the message is
    if(strncmp(message, "OK ", 3) == 0){
        if(!handleOK(message)){ //if the handling fails, repeat for grid as these are for initialization
            message_send(from, "KEY q"); //quit
            message_done();
            free(clientInfo);
            return false;
        }
    }else if(strncmp(message, "GRID ", 5) == 0){
        if(!handleGRID(message)){
            message_send(from, "KEY q");
            message_done();
            free(clientInfo);
            return false;
        }
        return false;
    }else if(strncmp(message, "GOLD ", 5) == 0){
        handleGOLD(message);
        return false;
    }else if(strncmp(message, "DISPLAY\n", 8) == 0){
        handleDISPLAY(message);
        return false;
    }else if(strncmp(message, "QUIT ", 5) == 0){
        endwin(); //quit curses and just print the end message, simplest
        fflush(stdout);
        fprintf(stdout, "\n%s\n", message);
        return true;
    }else if(strncmp(message, "ERROR ", 6) == 0){
        fprintf(stderr, "%s\n", message); //log error message to stderr
        return false;
    }else{
        fprintf(stderr, "unrecognized message: %s\n", message);
        return false;
    }
    return false;
}

/**************** handleOK ****************/
/* Handles OK message from the server, and assigns the players ID char to clientInfo
 */
static bool handleOK(const char* message){
    clientInfo->player = 0;
    if(sscanf(message, "OK %c", &clientInfo->player) != 1){ //if something is wrong with the message
        fprintf(stderr, "malformed OK message: %s", message);
        return false;
    }

    return true;
}

/**************** handleGrid ****************/
/* Given a GRID message from the server, this function initializes the ncurses screen
 * It is assumed that the given size from the server will be sufficient for future DISPLAYs
 */
static bool handleGRID(const char* message){
    int nRows;
    int nCols;
    //check for bad message
    if(sscanf(message, "GRID %d %d", &nRows, &nCols) != 2){ //validate message
        fprintf(stderr, "malformed GRID message: %s", message);
        return false;
    }
    clientInfo->nRows = nRows+1;
    clientInfo->nCols = nCols+1;
    initCurses(); //initialize the map
    initMap();
    return true;
}

/**************** handleGold ****************/
/* taking a GOLD message from the server, this function prints information about gold to each player
 * By checking the length of the output string, the function will choose
 *   whether or not to try and include "GOLD recieved x" in the message
 * it is assumed that the GOLD message matches the specific formatting
 */
static bool handleGOLD(const char* message){
    int addedGold;
    int currGold;
    int rmngGold;
    if(sscanf(message, "GOLD %d %d %d", &addedGold, &currGold, &rmngGold) != 3){ //validate message
        fprintf(stderr, "malformed GOLD message '%s'", message);
        return false;
    }else{
        int nCols = clientInfo->nCols;
        for (int i = 0; i < nCols; i++){ //go through first line, make blank
            mvaddch(0,i, ' ');
        }
        char msgStr[nCols];
        if(clientInfo->player == 0){
            snprintf(msgStr, nCols, "Spectator: %d nuggets unclaimed.", rmngGold); 
        }else{
            int msgLeng = snprintf(msgStr, nCols, "Player %c has %d nuggets (%d nuggets unclaimed).", clientInfo->player, currGold, rmngGold);
            char rmngGold_str[nCols];
            if(addedGold > 0){ //if gold
                if(nCols - msgLeng > 0){ //if there is space left to print gold
                    snprintf(rmngGold_str, nCols - msgLeng, "  GOLD recieved: %d", addedGold);
                    strcat(msgStr, rmngGold_str);
            }}
        }
        mvprintw(0,0, "%.*s", nCols, msgStr);
        refresh();
        return true;
    }
}

/**************** handleDisplay ****************/
/* using a DISPLAY message from the server, this function prints the display message to stdout.
 * it is assumed that the display message given by the server is valid and will fit to the server
 *  specified screen size
 */
static void handleDISPLAY(const char* message){
    const char* mapStr = message + strlen("DISPLAY");
    printMap(mapStr);
}

/**************** initCurses ****************/
/* initializes the ncurses window for the display
 */
static void initCurses(){
    initscr();
    int nRows_ = 0;
    int nCols_;
    getmaxyx(stdscr, nRows_, nCols_);
    if(nCols_< clientInfo->nCols || nRows_ < clientInfo->nRows){ //if there is not enough room in the screen
        endwin();
        fprintf(stderr, "\nunable to fit map of dimensions %d, %d, in screen of size %d, %d\n...terminating...\n\n", clientInfo->nRows, clientInfo->nCols, nRows_, nCols_);
        message_send(clientInfo->serverAddr, "KEY q"); //let the server know the player is quitting
        return;
    }
    cbreak();
    noecho();
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    attron(COLOR_PAIR(1));
}

/**************** initializeMessage ****************/
/* Sets up the message Module and prints server port number
 * code from support/miniserver.c credit David Kotz - June 2021
 */
static void printMap(const char* display){
    mvprintw(1, 0, "%s", display); //print starting from second line
    refresh();
}

/**************** initMap ****************/
/* Creates an empty map to initialize ncurses display
 */
static void initMap(){
        int max_nRows = 0;
        int max_nCols = 0;
        getmaxyx(stdscr, max_nRows, max_nCols);
        for (int row = 1; row < max_nRows; row++){
                for (int col = 0; col < max_nCols; col++){
                        move(row, col);
                        addch(' ');
                }
        }
        refresh();
}

/**************** clientInfo_new ****************/
/* allocate memory for the local clientInfo variables
 */
static clientInfo_t* clientInfo_new(){
        // allocate memory for array of pointers
        clientInfo_t* clientInfo = (clientInfo_t*)malloc(sizeof(clientInfo_t));
        if(clientInfo == NULL){
            fprintf(stderr, "memory alloc. error\n");
            exit(3);
        }
        clientInfo->nRows = -1; //make these values negative one, this will indicate if something is sent out of order
        clientInfo->nCols = -1;
        clientInfo->player = 0;
        return clientInfo;
}