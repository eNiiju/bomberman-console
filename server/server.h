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
#include <unistd.h>
#include <sys/types.h>

#include "../include/definitions.h"
#include "message.h"

/* ------------------------------------------------------------------------- */
/*                            Function prototypes                            */
/* ------------------------------------------------------------------------- */

/**
 * Set up and initialize the server.
 * @param path_to_map_file The path to file of the map to use.
 * @return true if successful, false otherwise
*/
bool setup(char* path_to_map_file);

/**
 * Cleans up the server before exiting the program.
 * @return true if successful, false otherwise
*/
bool clean_exit();

/**
 * Function handling players' actions.
 * It is created as a thread once a player joins the game.
 * It is responsible for the communication between the server and the client,
 * including receiving player's inputs and take them into account for the game.
 * @param arg Unused
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
 * Reads the map file and stores the data in the map
 * property of the global variable "game".
 * Store the player's spawn points in their respective
 * coordinates, but not in the map.
 * @param path_to_map_file The path to map's file.
*/
void retrieve_map_data(char* path_to_map_file);

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

/**
 * Function for the thread handling the "move" messages received
 * from a client.
 * @param arg (int*) The player's number.
*/
void* thread_player_message_move(void* arg);

/**
 * Function for the thread handling the "place bomb" messages received
 * from a client.
 * @param arg (int*) The player's number.
*/
void* thread_player_message_place_bomb(void* arg);

/**
 * Returns if a player can move in a given direction, in the current
 * game state in the global variable "game".
 * @param player_number The player's number.
 * @param direction The direction the player wants to move in.
*/
bool player_can_move(int player_number, int direction);

/**
 * Returns if a player can place a bomb in the current game state in the
 * global variable "game".
 * @param player_number The player's number.
*/
bool player_can_place_bomb(int player_number);

/**
 * Waits for the player's move cooldown to end, then moves the player
 * in the given direction.
 * @param arg (struct player_move*) The player's number and moving direction.
*/
void* thread_move_player(void* arg);

/**
 * Waits for the player's bomb cooldown to end, then places a bomb,
 * then waits for the bomb's explosion timer to end, then explodes and
 * put fire on the map, then waits for the bomb's fire to end,
 * then removes the fire.
 * @param arg (int*) The player's number.
*/
void* thread_place_bomb(void* arg);

/**
 * Returns if a player is being killed by a bomb's explosion
 * in the current game state of the global variable "game".
 * @param player_number The player's number.
*/
bool check_player_death(int player_number);

/**
 * Checks if the game is over, and if it is, sets the winner
 * and end the game. If the game is not over, it does nothing.
*/
void check_game_end(void);
