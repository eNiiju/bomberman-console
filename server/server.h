/**
 * Server header file.
 * @file server.h
 * @date 2022-11-16
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>

/* ------------------------------------------------------------------------- */
/*                          Constants & definitions                          */
/* ------------------------------------------------------------------------- */

#define MAX_PLAYERS 4
#define MIN_PLAYERS 2
#define DEFAULT_BOMB_RANGE 3
#define MAP_WIDTH 15
#define MAP_HEIGHT 15

/* ------------------------------------------------------------------------- */
/*                            Function prototypes                            */
/* ------------------------------------------------------------------------- */

/**
 * Initializes the server, mutexes, semaphores, default settings, etc.
 * @return true if successful, false otherwise
*/
bool init(void);

/**
 * Function handling signals sent to the server.
 * The protocol is defined in the README.md file.
 * @param signal_number The signal's number.
*/
void signal_handler(int signal_number);

/**
 * Function handling players' connections.
 * It is created as a thread once a player joins the game.
 * It is responsible for the communication between the server and the client,
 * including receiving player's inputs and take them into account for the game.
 * @param arg (short int) The player's number.
*/
void* thread_player(void* arg);

/**
 * Function handling the game.
 * It is created as a thread once the game starts.
 * It is responsible for the game's logic, including moving players, placing
 * modifying the map, etc.
 * @param arg Unused.
*/
void* thread_game(void* arg);
