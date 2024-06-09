# CS50 Nuggets
## Design Spec
### Team VI, Spring, 2024

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes a map and player module.


### User interface

See the [Requirements Spec](REQUIREMENTS.md) for both the command-line and interactive UI.
### Client
The user sees a string representing the world state and interacts with the keyboard. They are only able to see the objects within their line of view. Except for when they've visited a room. After visiting a room a user will be able to see the walls and empty spots, but not gold and other players after leaving. For more details see REQUIREMENTS spec. 
The Spectator also sees a string representing the world state. The only difference is that they see a master map. 

### Server
The only user interface seen in the server is after called with proper arguments, the port that the game is played on is displayed. 

### Inputs and outputs

#### Client
*Input*: 
Within the game the player has 9 options for valid inputs:
1. 'q' to quit the game
2. ‘h’ to move left
3. ‘l’ to move right
4. ‘j’ to move down
5. ‘k’ to move up
6. ‘y’ to move diagonally up and left
7. ‘u’ to move diagonally up and right
8. ‘b’ to move diagonally down and left
9. ‘n’ to move diagonally down and right

For each of the movement commands, a capital letter represents a "sprint"

The spectator has only 1 option for valid input: 
1. 'q' to quit the game

*Output*:
1. while the game is in progress, nothing shall be printed to `stdout`
2. any errors will be printed to `stderr`
3. while the game is in progress, all information from the server will be displayed in the game window made by ncurses
4. when the game concludes, player quits, or a fatal error occurs, the terminating message will be printed to `stdout`

The following is how different server messages will be handled:
1. `OK <playerID>`  
*Spectator*:
The spectator receives strings representing the world state. The first message received is the information regarding gold in the current world state. Then they will receive a string representation for the map in the current world state. 
Example: 
“Spectator: <NUMBER> nuggets unclaimed."
The string the spectator will see is a "master" map of the whole game. This map showcases where every player is and where all the gold is. It is updated everyone time a player moves until the game is over, then a leaderboard is finally shown.
    
*Player*: 
The player receives a string representation of the map. The first line in this string will be information regarding the world state.
Example: 
“Player <ID> has <NUMBER> nuggets (<NUMBER> nuggets unclaimed).”
    
The player starts off only being able to see what is around them in a their current line of sight. As they move their view is updated to see more of the map, new piles of gold, and other players. Everything that they have seen will still be outputted to them but if an area is not in their current line of sight the gold and other players will not be visible.

*Errors*:
Errors will be logged and sent to stderr. 
Ex: Error: Player already playing
Ex: Error: Too many players

### Server
#### Inputs 
The server's output is the clients input. And the server's input is the client's output. 
Specifically the server takes in `PLAY` `KEY` and `SPECTATE` messages. Based on these messages the server will respond differently. When given a `PLAY` message the server will check if it is from a non-playing address and port number and try to add them to the game. If it is a `SPECTATE` message the player will kick the current spectator if there is one, and add the new user as a spectator. If it is a `KEY` message the server will try to move the player if possible. Whenever someone moves in the game, the map will be updated for everyone. 
    
#### Outputs
The server's outputs are as follows: `OK` for when a new player successfully joins. `GRID` when a new player joins to explain the size of the map. `GOLD` whenever we update the map we send a new message of the remaining gold, gold collected, and gold stepped on. `DISPLAY` We send this message whenever someone moves to update the map for each player following this message is a string representation for the map. `QUIT` We send this when the game is over, or too many players are playing. We choose not to send an ERROR message when given unexpected input, to avoid having to send too many when being targetted by an attack.
    
    
### Functional decomposition into modules

We anticipate the following modules
1. *Map* 
    This module is meant to have the implementations for all functions that manipulate the map.
    The map module implements the map struct type.
2. *Player*
    This module is meant to have the implementations for all player actions and behaviors. 
    The player module implements the player struct type.
3. *Client*
    The *client* acts in one of two modes:
     1. *spectator*, the passive spectator mode described in the requirements spec.
     2. *player*, the interactive game-playing mode described in the requirements spec.
4. *Server*
    The server implements all game logic by handling inputs from users and calling relevent functions.

### Pseudocode for logic/algorithmic flow

#### Map module functions
    
*loadMap()*
Given a txt file representing a map, the function converts the map to a 2D array of strings.

Pseudocode:
    
    Initialize a 2D array where each slot is a char*
	    Loops over each character in loadMap:
			Based on each character in loadMap convert it to our defined types of tiles as defined above and store in the                 2D array. 
    Return the resulting 2D array. 
    
*mapDelete()*
Frees all the memeory associated with a given map

Pseudocode:
    
    Loop through each spot in the map
        Free each string in the map
    Loop through each col of the map
        Free each array for each col
    Loop thorugh each row of the map
        Free each array for each row
    Free the whole map struct
    
*loadGold()*
Takes in the 2D array of strings (map) and randomizes the placements of gold and updates the map

Pseudocode:

    Generate a random amount of gold to be distributed throughout the map
	While not all gold has been placed:
	    Randomly find spots in the map to place gold
        if it is a valid location
            add a random amount within the bounds of the gold left into the map array.
    
*getRandomLocation()*
From the map, returns a valid x,y coordinate in which the player can spawn 
    
Pseudocode:
    
    Initialize a random number for row
    Initialize a random number for column
    While map[rowNum][colNum] is not a valid place
	    Get a new random number for row
	    Get a new random number for column
    Return array of size two with rowNum and colNum
    
*setLocation()*
Given a map, x,y coordinate pair, and a string sets the tile at that x,y spot to the given string. Used by the player when they step off of a location and it needs to be replaced with what was their before
    
Pseudocode:
    
    Find the given spot
        Free the old string there
        Allocate memory for the new string
        Add that string at the given spot
    
*getLocation()*
Given a map, x,y coordinate pair it returns the string that is at that location.

Pseudocode:
    
    if the coordinates in the map dimensions
        Find the given spot
            Make a copy of the string
            Return the string at that spot
    
*convertMap()*
Given a map returns a string representation of it that can be sent to each player or spectator
    
Pseudocode:
    
    Initialize an empty string
    Loop through each spot in the map
        If its a number
            Replace it with a *
        Add it to the string
    return the string
    
*copyMap()*
Given a map, return a map of the same dimensions with empty spots everywhere instead. Used for computing visilibty 
    
Pseudocode:
    
    Create a new map with the same dimensions
    Loop through each spot
        initilize and empty string at each spot
    Return this map
    
*replaceGoldSpots()*
Given a map and current game state replace all players and gold piles on the map back to the original map state. This is used by visilibty 
    
Pseudocode:
    
    Loop through each spot
        if its a gold pile 
            replace with a '.'
        if its a player
            iterate through the hashtable to see whos id this is
            replace the spot with the player current location's tile

    
#### Player module functions
*initializePlayer()*
Given the gameState struct the player name, and the player's address, the function initializes a new player struct and chooses a random place in the map for the players initial location. The function then adds the player into the hashtable of players. 

Pseudocode:
    
    Iterate over the player hashatble and get the size of the hashtable
    If number is less than 26
        Set playerName as the name passed
		Get user ID from hashtable size
		Call getRandomLocation and set it as player initial location
		If success
		    Initialize playerGold as 0
		    initialize visited map by calling updateLineOfSight
		    update old location
            update new location
    
*updateLineOfSight()*
Given a master map, a player's map, a player, and a gamestate, the function computes which parts of the map are visible to the player from this location. This function handles the math computations for the visibility of the player. 
    
Pseudocode:
    call copyMap on player's map to clean the map
    for each location in the map
        if the location is visible to the given player location
            add the value at that location to the map
        else
            add an empty string as the value for the location to the map
    Return the map
    
*movePlayer()*
Given a player a string representing the direction (LEFT, RIGHT, UP, DOWN), and the master map, the function will update the player's location as well as his visible map.
    
Pseudocode:
    
    if direction is LEFT
        if spot to the left is an empty spot (.)
            update the old location to empty spot (.)
            update the new location as the spot to the left
            update player location
        else if the spot to the left has a player
            swap the location of the players
            update both players' location
    else if direction is RIGHT
        if spot to the right is an empty spot (.)
            update the old location to empty spot (.)
            update the new location as the spot to the right
            update player location
        else if the spot to the right has a player
            swap the location of the players
            update both players' location
    else if direction is UP
        if spot above is an empty spot (.)
            update the old location to empty spot (.)
            update the new location as the spot above
            update player location
        else if the spot above has a player
            swap the location of the players
            update both players' location
    else if direction is DOWN
        if spot below is an empty spot (.)
            update the old location to empty spot (.)
            update the new location as the spot below
            update player location
        else if the spot below has a player
            swap the location of the players
            update both players' location
    call updateWorld            

*playerQuit()*
Given a player, this function handles the case when a client (only player) decides to quit.
    
Pseudocode:
    
    If player is a valid player
        change player's active status to false
        send a quit message to player's client

*playerDelete()*
Given a player, the function frees all of the required values from a player and then deletes the player.
    
Pseudocode:

    call mapDelete on player map
    for each instance variable of the player
        free the instance variable
    free the player

*playerPrint()*
Given a player, the function prints out the passed player's information.
    
Pseudocode:
    
    print player information header
    print player ID
    print player name
    print player location
    print player current spot tile
    print player address
    print player status
    print player gold count
    call convertMap on player map
    print player map
    
    
#### Client module functions

*sendMessage()*
Takes in the user keystroke and sends the keystroke to the server so the player can be updated. This includes 8 movement keys plus quit

Pseudocode:
    
    Checks user input using ncurse 
	If the user inputs a valid key:
	    Sends the message to the server

*receiveMessage()*
Receives a String message from the server, parse for what information it is carrying, and then handles the message
Pseudocode:
    
    if the message is for OK
        save playerID
    if the message is for GRID
        save GRID info and init ncurses
    else if msg is for GOLD
        print GOLD info to screen
    else if msg is for DISPLAY
        print the map w printMap
    else if the msg is QUIT
        close screen
        print exit message
        break communication loop
    else
        its a malformed message, log to stderr and continue
    
*printMap()* 
Once the user recieves a map of their visibility as a string it is printed out

Pseudocode:
    Prints provided string to stdout
    
*The client will run as follows*:

	execute from a command line per the requirement spec
	parse the command line
    validate parameters
	Initialize the 'message' module
    Wait on message from server
	While the client-server communication is active
        continue to display the latest message recived from the server
        send any input to the servers
    close server
	clean up
    
#### Server module functions
    
*parseArgs()*
Given the commandline arguments, parses them and ensures given a readable map, and positive seed (if given a seed)
    
Pseudocode:
    ensure given 2 or 3 commandline arguments
    ensure map file is readable
    if given a seed ensure it is positive and an int
    
*initalizeGameState()*
Given a txt map file and an integer for the total amount of gold, the function
handles initalizing the data structures needed to prepare for the game

Pseudocode:
    
    call loadMap with txt file of map
    call loadGold with resulting map
    Initalizes the hashtable to store players
    Initalizes gameState struct with hashtable, map, and gold
    
*messageLoop()*
The function sits open, listening for a message from a user. When it receives a message sends it to receiveMessage.
    
Pseudocode:
    
    while receiving messages. 
        Send each message to handleMessage
    
    
*gameOver()*
Takes the gameState.
Once all the gold in the game is gone a new screen is outputted to the user. Everyone will recive a leaderboard.
    
Pseudocode:
    
    If amount of gold equals 0
        Create the final table
        Call sendMesage to send a leaderboard to everyone
 
*handleMessage()*
Receives a message from the user and handles logic of how we should act based on message received
    
Pseudocode:
    
    If message is valid:
        call appropriate function based on valid message
    

*updateWorld()*
Given the gameState, the function handles the logic for trying to update the world state by using Map module, player module, remainingGold.
    
Pseudocode:
    
    For each player
		call computeUserVisibility
		Call send message for each player with their updated visibility
	If there is a spectator 
        send the master map to the spectator
    

*The server will run as follows*:

	execute from a command line per the requirement spec
	parse the command line, validate parameters
	call initializeGame() to set up data structures
	initialize the 'message' module
	print the port number on which we wait
	call message_loop(), to await clients
    handle all incoming messages appropriatly
	call gameOver() to inform all clients the game has ended
	clean up

### Major data structures

The major data structures are as follows:

#### Player
* A player is a struct where we store: player ID (string), player location (an array with 2 integers, an X value and a Y value), playerGold (int), visitedMap (2D array of strings).
* Player location is an integer {X,Y} value pair for the current position of the player on the map
* Visited map is the places the user has already been, that should have unique visibility as defined in the REQUIREMENTS spec.


#### Map
* A map is just a 2D array of size NR * NC, where each slot represents a space on the map.
* The map struct will also store 2 ints for the number of rows and number of columns of the map
    
#### gameState
* A gameState struct holds the current master map of the game, the master hashtable of the game, and an int for the total amount of gold that is remaining on the map. As well it holds the spectators address if we have a spectator. The master map is a 2D array of Strings that stores the all the map info. The master hashtable has player IDs as the key and the values are the corresponding player struct.

#### clientInfo
* clientInfo manifests in a global variable for the client, and holds the number of rows and columns, the player ID, and the server address.
    
##### Additional Data Structures
* We anticipate using the Hashtable data structure located in the libcs50 library. In addition, the data structures required for client server communication featured in message.h will play a major role in implementation.

### Testing Plan
We will test some functions for the modules individually. 

Specific modules contain functions that are standalone, for example loadMap, loadGold, from the map module, as well in the player module computeUserVisibility can be tested with just a Map and a Player struct. These can and will also be tested individually with valgrind. 

We want to ensure these functions are working exactly to spec since they are the most important and likey most complicated. So we will test them the most extensively.

Once we have tested individually modules we will complete integration testing to see how modules work together. 
    
We will test server with several different cases of users, some giving invalid keystrokes, some trying to go through walls and perform other invalid operations. 


We will also test several edge case scenarios, such as users bumping into each other, users sprinting into each other and users sprinting to walls.
    
Further, we will test the result of maps that are either too large or too small being sent to the client.
    
Then we will test with valgrind and ensure the only memory leaks we have is with ncurses, and that we have no errors.