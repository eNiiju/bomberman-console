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

#include "../include/definitions.h"
#include "message.h"

/* ------------------------------------------------------------------------- */
/*                            Function prototypes                            */
/* ------------------------------------------------------------------------- */

/**
 * Set up and initialize the server.
 * @return true if successful, false otherwise
*/
bool setup(void);

/**
 * Function handling players' actions.
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
 * @param pid_client The client's PID.
 * @return true if successful, false otherwise.
*/
bool create_player(pid_t pid_client);

/**
 * Returns if the client is in the game or not.
 * @param pid_client The client's PID.
 * @return true if the client is in the game, false otherwise.
*/
bool client_in_game(pid_t pid_client);

/**
 * Function for the thread handling the "connection" messages.
 * It is created as a thread once the server starts.
 * @param arg Unused.
*/
void* thread_main_message_queue(void* arg);
