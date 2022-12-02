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
