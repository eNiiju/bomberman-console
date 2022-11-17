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
 * Set up and initialize the server, mutexes, semaphores, default settings, etc.
 * @return true if successful, false otherwise
*/
bool setup(void);

/**
 * Function handling signals sent to the server.
 * The protocol is defined in the README.md file.
 * @param signal_number The signal's number.
 * @param info The signal's informations.
*/
void signal_handler(int signal_number, siginfo_t* info, void* ucontext);

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
 * Creates a player, initializes it's values and adds it to the game.
 * @param client_pid The client's PID.
*/
void create_player(pid_t client_pid);
