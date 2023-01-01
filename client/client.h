/**
 * Client header file.
 * @file client.h
 * @date 2022-11-16
*/

#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <ncurses.h>

#include "../include/definitions.h"
#include "message.h"
#include "display.h"

/* ------------------------------------------------------------------------- */
/*                            Function prototypes                            */
/* ------------------------------------------------------------------------- */

/**
 * Function for the thread that displays the game state.
 * @param arg Unused.
*/
void* thread_display(void* arg);

/**
 * Function for the thread that handles the user inputs.
 * @param arg Unused.
*/
void* thread_inputs(void* arg);

/**
 * Function for the thread that handles the "game state" messages
 * received from the server.
 * @param arg Unused.
*/
void* thread_message_game_state(void* arg);

/**
 * Function for the thread that handles the "game end" messages
 * received from the server.
 * @param arg Unused.
*/
void* thread_message_game_end(void* arg);
