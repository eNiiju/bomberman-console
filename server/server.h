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

#include "message.h"

/* ------------------------------------------------------------------------- */
/*                          Constants & definitions                          */
/* ------------------------------------------------------------------------- */

#define MAX_PLAYERS 4
#define MIN_PLAYERS 2
#define DEFAULT_BOMB_RANGE 3
#define BOMB_TIMER 3
#define MAP_WIDTH 15
#define MAP_HEIGHT 15

struct coordinates {
    unsigned int x;
    unsigned int y;
};

struct player {
    pid_t client_pid;
    bool alive;
    struct coordinates coords;
    unsigned int bomb_amount;
    unsigned int bomb_range;
};

/* ------------------------------------------------------------------------- */
/*                            Function prototypes                            */
/* ------------------------------------------------------------------------- */

/**
 * Set up and initialize the server.
 * @return true if successful, false otherwise
*/
bool setup(void);

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

/**
 * Creates a player if it's not already added, initializes it's values and
 * adds it to the game.
 * @param client_pid The client's PID.
 * @return true if successful, false otherwise.
*/
bool create_player(pid_t client_pid);

/**
 * Returns if the client is in the game or not.
 * @param client_pid The client's PID.
 * @return true if the client is in the game, false otherwise.
*/
bool client_in_game(pid_t client_pid);
