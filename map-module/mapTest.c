/* 
 * mapTest.c - tests all map functionality
 *
 * Implemented according to Implementation Spec
 * 
 *
 * TEAM VI, Spring 2024
 */

#include "../support/hashtable.h"
#include "../corestructures.h"
#include "map.h"
#include "../player-module/player.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <strings.h>
#include <ctype.h> 
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

// Main function for testing
int 
main(int argc, char *argv[]) 
{

    // Testing load map
    map_t* map = loadMap(argv[1]);
    if (map == NULL) {
        fprintf(stderr, "Failed to load map\n");
        return 1;
    }

    // Print map without gold loaded yet
    for (int i = 0; i < map->numRows; i++) {
        for (int j = 0; j < map->numCols; j++) {
            printf("%s", map->mapitem[i][j]); 
        }
        printf("\n");
    }

    // Testing loadGold
    loadGold(map);  // Distribute gold on the map

    // Print the map with gold distribution
    for (int i = 0; i < map->numRows; i++) {
        for (int j = 0; j < map->numCols; j++) {
            printf("%s", map->mapitem[i][j]); 
        }
        printf("\n");
    }

    // Testing convert Map
    printf("\nConverted Map (Converts map into a string and prints this string out):\n"); // Printing map string
    char* convertedMap = convertMap(map, NULL);
    if (convertedMap != NULL) {
        printf("%s", convertedMap);
    }
    free (convertedMap); // free string

    // Print 5 random locations the player can spawn
    printf("\nRandom Locations the player could spawn in:\n");
    for (int k = 0; k < 5; k++) {
        int* location = getRandomLocation(map);
        if (location != NULL) {
            printf("Random location %d: (%d, %d)\n", k+1, location[0], location[1]);
        }
        free(location);
    }

    // Test getLocation at various points
    int testPoints[25][2];
    int index = 0; // Index for filling the array

    for (int i = 2; i < 7; i++) {
        for (int j = 2; j < 7; j++) {
            testPoints[index][0] = i; // Set the x-coordinate
            testPoints[index][1] = j; // Set the y-coordinate
            index++; // Increment the index to move to the next spot
        }
    }

    printf("\nTesting getLocation:\n");
    for (int k = 0; k < 25; k++) {
        int x = testPoints[k][0];
        int y = testPoints[k][1];
        char* locationContent = getLocation(map, x, y);
        if (locationContent != NULL) {
            printf("Content at (%d, %d): %s\n", x, y, locationContent);
            free(locationContent);
        }
    }
    
    // Set new values at specific locations
    setLocation(map, 1, 1, "test1");
    setLocation(map, 14, 8, "test2");
    setLocation(map, 5, 5, "test3");

    // Print the map after modifications
    printf("\nMap After Modifications:\n");
    for (int i = 0; i < map->numRows; i++) {
        for (int j = 0; j < map->numCols; j++) {
            printf("%s", map->mapitem[i][j]);
        }
        printf("\n");
    }

    // Testing making the empty map
    map_t* copiedMap = copyMap(map);
    printf("\nPrinting Empty map of the same size:\n");
    for (int i = 0; i < copiedMap->numRows; i++) {
        for (int j = 0; j < copiedMap->numCols; j++) {
            printf("%s", copiedMap->mapitem[i][j]);
        }
        printf("\n");
    }

    // Cleanup
    mapDelete(map);
    mapDelete(copiedMap);
    return 0;
}
