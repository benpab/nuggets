# CS50 Nuggets
## Implementation Spec
### Team VI, Spring, 2024

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes map, and player modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Plan for division of labor

Before we all get started with coding we will each present on our plans for our specific functions. We will make sure we talk about how to coordinate our parts together. That way it is clear what our functions expect and what they return. Before each of us start coding we will make sure to get our plan approved. As well this part will include how we will test our functions to prove they work as intended. 

Server logic and overall flow: Cameron 
Map Module: Kevin
Player Module: Oskar
Client Module and computeVisibility function: Ben

We will help each other whenever someone gets stuck. Throughout this process we all also make sure to meet often and check in with each other to make sure our code is compatable. As well if some parts get finished easier/faster than others we will help with each others work.

## Map

### Data structures

A map is a 2D array (NR * NC) of string where each slot in an inner array represents a spot on the map (this includes all the characters that represents things like walls, corners, empty space, etc on the map). This data structure also holds the numRows and numCols of the map.

```c
typedef struct map {
    char*** mapitem;
    int numRows;
    int numCols;
} map_t;
```

So each slot will store a string representing that spot. 
Ex:
[["."], ["."], ["10"], ["#"]] 
where a numerical string represents the amount of gold stored in that spot.

### Definition of function prototypes with corresponding Pseudocode

#### `loadMap`

Given a txt file that represents a playable map, this function converts into our 2D array map datastructure and returns the map. 
```c
map_t* loadMap(const char* mapTxtFile);
```

Pseudocode:

    Initialize a 2D array where each slot is a char*
    Loops over each character in mapfile:
		Based on each character in mapfile convert it to our defined types of tiles as defined in the design spec and store in the 2D array
    save the 2D array into a map_t
    Return the resulting map_t* 

#### mapDelete
Frees all the memeory associated with a given map ensuring no memory leaks.

```c
void mapDelete(map_t* map)
```
    
Pseudocode:
    
    Loop through each spot in the map
        Free each string in the map
    Loop through each col of the map
        Free each array for each col
    Loop thorugh each row of the map
        Free each array for each row
    Free the whole map struct

#### `loadGold`

Called after the Map has been initalized with the current gameState, to initalize the map with proper amount of gold and store the result of this in the gameState. Makes sure the number of gold piles is greater than the minimum amount of gold pules and less than the maximum.

```c
void loadGold(map_t* map);
```
Pseudocode:
    
    initalize totalGold to be equal to the constant GoldTotal
    initalize count variable to count numPiles
    While totalGold > 0 and GoldMinNumPiles < numPiles < GoldMaxNumPiles:
        Use the seed number to find a random spot on the map
            if it is a valid location
                add a random amount within the bounds of the gold left into the map array.


#### `getRandomLocation`

Takes in the game state after we have setup the map to find a valid place to put a player. This returns an x and y point as a two item array of integers. The calculations are done with the seed number. 

```c
int* getRandomLocaton(map_t* map);
```
Pseudocode:

    initialize an empty 2d array to store all valid (x,y) spawn points
    loop over the given map
        if the spot is valid
            add it to the array
    pick a random point from this array
    return the point as an array of two ints

#### `convertMap`

Takes in the map and converts it to a string representation by adding newlines when necessary. Also takes in the playerID so it can be replaced with a @ for the corresponding player
The resulting string will be [nr+1] * [nc] + 1 large. This is to account for a newline after each row, and a terminating '\0' at the end of the string.

```c
char* convertMap(map_t* map, char* playerID);
```

Pseudocode:
    
    initalize a string that is [nr+1] * [nc] + 1 large
    Loops over the outermost array of the map:
        loop over each element in the row of the map:
            if the string is a numerical value:
                add a '*' to the resulting string
            if the string is the player's ID
                add a '@' to the resulting string
            else:
                add the current string to the resulting string
        add a newline to the resulting string
    add a '\0' to the resulting string
    return the resulting string
    
#### `getLocation`

Takes in the master map and a pair of ints to return a string of what string (|, +. #, 123, etc...) is at that slot in the 2D map array

```c
char* getLocation(map_t* map, int x, int y);
```

Pseudocode:

    if x and y are within map's size
        return item at (x, y) on the map
    else
        print error message
        return nothing
        
#### `setLocation`

Takes in map, x,y coordinate pair, and a new tile to update that spot on the map with the new tile

```c
char* getLocation(map_t* map, int x, int y, char* newTiles);
```

Pseudocode:

    Find the given spot
        Free the old string there
        Allocate memory for the new string
        Add that string at the given spot
        
#### `copyMap`
Given a map, return a map of the same dimensions with empty spots everywhere instead. Used for computing visilibty 
    
```c
map_t* copyMap(map_t* original);
```
    
Pseudocode:
    
    Create a new map with the same dimensions
    Loop through each spot
        initilize and empty string at each spot
    Return this map
    
#### `replaceGoldSpots`
Given a map and current game state replace all players and gold piles on the map back to the original map state. When it sees a letter it uses the player hashtable in currState to find which player this is so it can replace the tile with the player's current location tile. This is used when checking visilibty 
    
```c
void replaceGoldSpots(map_t* map, gameState_t* currState)
```
    
Pseudocode:
    
    Loop through each spot
        if its a gold pile 
            replace with a '.'
        if its a player
            iterate through the hashtable to see whos id this is
            replace the spot with the player current location's tile
___

## Player

### Data structures

A player is a struct that represent someone playing the game. In it we store: player ID (string), player location (an array with 2 integers, an X value and a Y value), playerGold (int), visitedMap (map), the current spot they are standing on (string), and whether they have quit or not.

```c
typedef struct player {
    char* playerID;
    char* playerName;
    int* playerLocation;
    int playerGold; 
    int collected;
    map_t* playerVisitedMap;
    addr_t playerAddress;
    char* currentSpot;
    bool isPlaying;
} player_t;
```

### Definition of function prototypes with corresponding Pseudocode

#### `initializePlayer`

The function initializes a new player struct and chooses a random place in the map for the players initial location based on the seed. The function then adds the player into the hashtable of players. We assume gamestate and address are valid.

```c
bool initializePlayer(gameState_t* currState, char* playerName, addr_t playerAddress);
```
Pseudocode:

    Allocate memory for the player
    Iterate over the player hashtable and get the size of the hashtable
    If number is less than 26
        It player name is valid
            Set playerName as the name passed
            Get player ID from hashtable size
            Call getRandomLocation and set it as player initial location
            If success
                Initialize the players currentSpot to whatver character was on the spot they were put on
                Initialize playerGold as 0
                Initialize collectedGold as 0
                initialize visited map as an empty map
                Call updateLineOfSight to get visited map
                initialize playerAddress with the address passed
                set isPlaying to true
                add player to player hashtable with a string representation of playerAddress as the key
                Call setLocation with the player's location and ID
                return true
            else
                send an error message
                free name
                free player ID 
                free player
                return false
        else
            send an error message
            free player
            return false
    else:
        send an error message saying too many players
        free player
        return false


#### `updateLineOfSight`

The function computes which parts of the map are visible to the player from this location. This function handles the math computations for sightline of player. We assume all parameters passed are valid.


```c
bool updateLineOfSight(map_t* masterMap, map_t* visibleMap, player_t* player, gameState_t* currstate);
```
Pseudocode:

    get number of rows and columns in the map
    get playerID
    get player Location
    if masterMap and visibleMap are not NULL
        get player's x and y coordinates
        remove all gold from the player's map
        call setLocation with player's coordinates and ID
        for each x,y point in the map
            if point is visible
                add the value at that point to the map
            else
                add an empty string as the value for the location to the map
        if success 
            return true
    
#### `movePlayer`

The function updates the player's location, their gold, as well as the visible map based on the keystroke given. We assume all parameters passed are valid.

```c
bool movePlayer(gameState_t* currState, player_t* player, char* keystroke);
```
Pseudocode:
    
    normalize the keystroke (all lowercase)
    if keystroke is "Q"
        call playerQuit
    else if keystroke is "h"
        if another player is on the spot to the left
            swap the location for both players
        else if spot to the left is a valid location
            if spot to the left has gold
                increment the player's gold count by the correct amount
            move player to the left
    else if keystroke is "l"
        if another player is on the spot to the right
            swap the location for both players
        else if spot to the right is a valid location
            if spot to the right has gold
                increment the player's gold count by the correct amount
            move player to the right
    else if keystroke is "k"
        if another player is on the spot above
            swap the location for both players
        else if spot above is a valid location
            if spot above has gold
                increment the player's gold count by the correct amount
            move player to the spot above
    else if keystroke is "j"
        if another player is on the spot below
            swap the location for both players
        else if spot below is a valid location
            if spot below has gold
                increment the player's gold count by the correct amount
            move player to the spot below
    else if keystroke is "y"
        if another player is on the spot to above to the left
            swap the location for the players
        else if spot above to the left is a valid location
            if spot above to the left has gold
                increment the player's gold count by the correct amount
            move player to spot above to the left
    else if keystroke is "u"
        if another player is on the spot to above to the right
            swap the location for the players
        else if spot above to the right is a valid location
            if spot above to the right has gold
                increment the player's gold count by the correct amount
            move player to spot above to the right
    else if keystroke is "b"
        if another player is on the spot to below to the left
            swap the location for the players
        else if spot below to the left is a valid location
            if spot below to the left has gold
                increment the player's gold count by the correct amount
            move player to spot below to the left
    else if keystroke is "n"
        if another player is on the spot to below to the right
            swap the location for the players
        else if spot below to the right is a valid location
            if spot above to the right has gold
                increment the player's gold count by the correct amount
            move player to spot below to the right
    call setLocation with old location and player spot tile
    get new spot's tile value
    call setLocation with new location and new spot tile

#### `playerQuit`

This function handles the case when a client (only player) decides to quit.

```c 
void playerQuit(player_t* player);
```

*Pseudocode:*
    
    if player is not NULL
        set player's isPlaying boolean to false
        send quit message to player

#### `playerDelete`

The function frees all of the required values from a player and then deletes the player. We assume the player is a valid player.


```c
void playerDelete(void* item);
```
Pseudocode:

    get the player
    call mapDelete on player's map
    free the player's name
    free the player's ID
    free player's location
    free player's current spot tile
    free the player

#### `playerPrint`

The function prints out the passed player's information. We assume the player is a valid player.


```c
void playerPrint(player_t* player);
```
Pseudocode:

    print player information header
    print playerID
    print playerName
    print playerLocation
    print player currentSpot
    print playerAddress
    print playerStatus
    print playerGoldCount
    
    get playerVisitedMap
    call convertMap on playerVisitedMap
    print playerVisitedMap
    free playerVisitedMap
___


## Client

The client handles communicating with the the server. It sends messages aswell as recieves messages from the server that are processed to provide the user with the game interface

### Data structures

    typedef struct clientInfo { //way to store map size and player icon for printing
        int nRows;
        int nCols;
        char player;
        addr_t server;
    } clientInfo_t;    
    static clientInfo_t* clientInfo_new()

clientInfo stores the number of rows, columns, the playerID of the player, and the server address. One such global struct is used to handle sending and receiving messages

### Definition of function prototypes with corresponding Pseudocode

The main function that handles the logical flow of the program.

```c
int main(const int argc, char* argv[]);
```

Pseudocode:

	execute from a command line per the requirement spec
	parse the command line
    validate parameters
	Initialize the 'message' module
    send c
    Wait on message from server
	While the client has not input 'q' to quit
        continue to display the latest message recived from the server
    close server
	clean up


*sendMessage(char keyStroke, addr_t* serverAddress)*

Takes in the user keystroke and sends the keystroke to the server so the player can be updated. This includes 8 movement keys plus quit

```c
void sendMessage(char keyStroke, addr_t* serverAddress);
```

Pseudocode:
    
    Checks user input using ncurse 
	If the user inputs a valid key:
	    Sends the message to the server

*receiveMessage(char* message)*
Receives a String message from the server. Processes the first word in the mesage the user sends and handles how that message will be handled.
    
```c
void recieveMesage(char* message);
``` 
    
Pseudocode:
    
    if first word is OK:
        saves playerID
    if first word is GRID:
        create a string to store the map based on nrows and ncols
        if first word is DISPLAY:
        prints out the second string
    if first word is QUIT:
       exit curses, 
       print the explanation followed by a newline
       break server-client communication
    if first word is GOLD:
        process amount of gold and print to screen
    if first work is ERROR:
        print error to stderr
    else:
        it is a malformed message, record to stderr
        
      
---

## Server

### Data structures

```c
typedef struct gameState {
    
    map_t* masterMap;
    hashtable_t* players;
    int goldRemaining;
    int randonNumber;
    addr_t* spectator;
} gamestate_t;
```

A gameState stores information regarding the state of the current world that is the game. The masterMap contains everything from players represented by ids, to gold represented by numerical strings. 
The players hashtable stores all of our players.
The goldRemaining is how much gold is left on the map, to be collected by the players. Once the goldRemaining is 0, the game ends. 
The randomNumber stores the random number to allow for consistant testing. 

### Definition of function prototypes

### `main`: 

The main function that handles the logical flow of the program.

```c
int main(const int argc, char* argv[]);
```

*Pseudocode:*
    
    calls parseArgs
    calls initalizeGameState
    calls loadGold
    call initializeMessage
    call message_loop
    call message_done
    if success
        return 0
    else
        return 1
        
#### `initializeMessage`:

A function to initialize the message module. This function also prints out the port the game will run on.

```c 
static bool initializeMessage();
```

*Pseudocode:*
    
    call message_init with null
    print the port number

#### `parseArgs`:

A function to parse the command-line arguments. This function returns the seeded random number.

```c
static int parseArgs(const int argc, char* argv[]);
```

*Pseudocode:*

    validate commandline
    verify map file can be opened for reading
    if seed provided
   	 verify it is a valid seed number
   	 seed the random-number generator with that seed
    else
   	 seed the random-number generator with getpid()
     

#### `initializeGameState`:

This functions initializes the current state of the game by allocating memory for the masterMap and hashtable of players.
This function checks that allocation works properly by checking for null pointers

```c
static gameState_t* initializeGameState(int gold, map_t* currMap);
```

*Pseudocode:*
    
    allocate space for a gameState_t*
    initialize a hashtable for players
    stores the currMap in gameState
    stores the gold in gameState
    initialize the spectator to be empty
    return the gameState.
    
#### `gameStateDelete`

This function frees all necessary allocated data used in a gameState

```c
static void gameStateDelete(gameState_t* gameState);
```

*Pseudocode:*
    
    try to free the hashtable of players
    try to free the map
    try to free the gameState


#### `handleMessage`

This function receives a message from the user and the user address, and decides what to do with that message. That is, the function determines if a new player should be added, spectator added, or if a key input needs to be handled.

```c
static bool handleMessage(gameState_t* gameState, const addr_t from, const char* message);
```

*Pseudocode:*
    
    if message is PLAY not given a name:
        log an error
    if message is PLAY <name>:
        call initalizePlayer
        if false
            call messageSend with error message
    if message is SPECTATE:
        if we have a spectator:
            send them a QUIT message
        set the gameState's spectator with current addr
    if message is KEY <KEY>:
        if this is a spectator:
            call handleSpectator
        else:
            call movePlayer with gameState, player, and char
            call updateWorld

#### `handleSpectator`

This function handles keystrokes from a spectator.
The only keystroke we envision handling is 'q' to quit, we ignore all others, but log them.

```c 
static void handleSpectator(gameState_t gameState, char keystroke);
```

*Pseudocode:*

    if the keystroke is 'q':
        set the spectator address in gameState to null
    else:
        log the input as it is a non-valid message
    
#### `updateWorld`

This function updates the screen for each player whos isPlaying instance is still true. That is, it updates the world for each player by sending them a new version of their map, and gold.
```c
static void updateWorld(gameState_t* gameState);
```

*Pseudocode:*

    if remaining gold is 0
        set global variable to true
        call gameOver
    else
        for each player that is playing in player hashtable:
            send their updated gold
            send their updated map
        if there is a spectator
            send the updated gold
            send the updated master map

#### `gameOver`

This function creates the table with the end game stats and sends this message to all players and the spectator. 

```c 
static void gameOver(gameState_t* gameState);
```

*Pseudocode:*
    
    initalize a string for table
    loop over all players:
        add the playerID, their score, their name and a newline to the table string
    
    loop over all players:
        call sendMessage with "QUIT GAME OVER: " and the string table as one string

---

## Testing plan

### Unit testing

We will test each module on its own to ensure they are producing the desired result. 
There are several functions that can be tested in isolation and we will test them extensively individually and as a unit. 
We will test the map module by making sure loadMap properly moves a map to a 2d array. Then we will test that loadGold works by giving it a premade map and specific seed and ensuring it is putting a numerical string in the right slot. From here we can test functions like getRandomLocation, setLocation, and getLocation, to make sure they are returning the proper output.
Furthermore, we will test the other functions in map individually by using a known seed to create a constant gamestate and ensure that these functions work as supposed to with that gamestate. 

We will test the Player module in a similar way. Using the preset gameState we will initalizePlayer and ensure they are being stored properly. 
computeUserVisibility and updateLineOfSight will be tested the most extensively and will also be tested with preset gamestates but multiple different ones. Some with multiple players in view and others with being in a corridor. 

The client will be originally tested with logging to ensure we are sending the properly formatted messages. We will first test the commandline arguments and that it can only be run with valid arguments. As well we will test it with premade messages and all different commands, to ensure proper handling.

The server will be individually tested by making sure it only accepts valid commandline arguments. It will also be tested to make sure it can deal with receiving premade messages, then we will try to send premade messages as well. Once the individual functions have been tested we will test the server and client together with specific logging to help us ensure everything is receiving and working as intended. 

Once we are confident in each individual module and the testing provides us this confidence, we will move on to testing it together. 

### Integration testing

We will test the complete program by using the same premade seed we had done unit testing with, and try it with the entire program. Then we will create more seeds where we know exactly the output and ensure everything works as it should. 
Once we are confident in the general functionality we will test edge cases. These cases are when: two players bump into each other (both in empty space as well as in corridors), two players try to grab gold around the same time, a player tries to run into a wall while another player runs into them. 

### System testing

We will test the client and server together by having multiple terminals open and playing with several different clients on the same server. We will also test this by connecting with different devices and utizilizing different devices to host the server. This will ensure that we are sending the proper map to the proper client and it is working as intended. As well, we will complete our edge case testing with multiple clients to ensure everything works as we want. 

## Limitations

- Since we represent each spot on the map as a string it will take more time to compare and check if the spot is a numerical value. This could be optimized by using hexadecimal numbers. However, we think this would complicate our code and chose the simpler approach.
- We continue to store players even after they have quit the game, this can be a waste of resources but is necessary for the game over screen and displaying their stats. 

## Errors handling

We check for allocation errors wherever we alloc memory. 

Since our game depends on strings and players being represented on the map, and these are mostly what we alloc, if there are any allocation errors, the map and overall game will act in undesirable ways. Because of this we do not allow the game to continue to run after receiving a null pointer from an alloc error. If we did the game would not be playable. So we print an error message to stderr, and exit. If there is a single alloc error with creating a new player, we send them a quit message and continue to play the game. 