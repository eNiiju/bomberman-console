/**
 * message.c header file.
 * @file message.h
 * @date 2022-11-23
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "../include/definitions.h"
#include "../include/message_structures.h"

/* ------------------------------------------------------------------------- */
/*                            Function prototypes                            */
/* ------------------------------------------------------------------------- */

/**
 * Create a message queue.
 * @param game_code The game code.
 * @return The message queue ID if successful, -1 otherwise
*/
int create_message_queue(int game_code);

/**
 * Send a response to the client regarding it's connection request.
 * @param main_msqid The main message queue ID
 * @param success true if the request was successful, false otherwise
 * @param pid_client The PID of the client to send the response to
 * @return true if the message was sent successfully, false otherwise
*/
bool send_connection_response(int main_msqid, bool success, pid_t pid_client);

/**
 * Send a response to the client regarding it's request.
 * @param client_msqid The destination client's message queue ID
 * @param success true if the request was successful, false otherwise
 * @return true if the message was sent successfully, false otherwise
*/
bool send_response(pid_t client_msqid, bool success);

/**
 * Send a message to the client containing the game state.
 * It also indicates that the game has started.
 * @param client_msqid The destination client's message queue ID
 * @param game_state The game state
 * @return true if the message was sent successfully, false otherwise
*/
bool send_game_state(pid_t client_msqid, struct game game_state);

/**
 * Send a message to the client to tell it that the game has ended.
 * @param client_msqid The destination client's message queue ID
 * @param winner The winner's player index
 * @return true if the message was sent successfully, false otherwise
*/
bool send_game_end(pid_t client_msqid, int winner);
