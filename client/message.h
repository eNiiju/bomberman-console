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
 * Sends a "connection" message to the server.
 * @param main_msqid The main message queue ID.
 * @param pid The client's PID.
 * @return true if the message was sent successfully, false otherwise
*/
bool send_message_connection(int main_msqid, pid_t pid);

/**
 * Sends a "move" message to the server.
 * @param client_msqid The client's message queue ID.
 * @param direction The direction the player wants to move.
 * @return true if the message was sent successfully, false otherwise
*/
bool send_message_move(int client_msqid, int direction);

/**
 * Sends a "place bomb" message to the server.
 * @param client_msqid The client's message queue ID.
 * @return true if the message was sent successfully, false otherwise
*/
bool send_message_place_bomb(int client_msqid);
