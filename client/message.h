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
#include "../include/message_types.h"

/* ------------------------------------------------------------------------- */
/*                            Function prototypes                            */
/* ------------------------------------------------------------------------- */

/**
 * Sends a "connection" message to the server.
 * @param msqid The message queue ID.
 * @param pid_client The client's PID.
 * @return true if successful, false otherwise
*/
bool send_message_connection(int msqid, pid_t pid);

/**
 * Sends a "disconnection" message to the server.
 * @param msqid The message queue ID.
 * @param pid_client The client's PID.
 * @return true if successful, false otherwise
*/
bool send_message_disconnection(int msqid, pid_t pid);

/**
 * Sends a "move" message to the server.
 * @param msqid The message queue ID.
 * @param pid_client The client's PID.
 * @param direction The direction the player wants to move.
 * @return true if successful, false otherwise
*/
bool send_message_move(int msqid, pid_t pid, int direction);

/**
 * Sends a "place bomb" message to the server.
 * @param msqid The message queue ID.
 * @param pid_client The client's PID.
 * @return true if successful, false otherwise
*/
bool send_message_place_bomb(int msqid, pid_t pid);

/**
 * Receive a response message from the server.
 * @param msqid The message queue ID.
 * @param pid_client The client's PID.
 * @return The response's success value.
*/
bool receive_message_response(int msqid, pid_t pid);
