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

bool send_message_connection(int msqid, pid_t pid);

bool send_message_disconnection(int msqid, pid_t pid);

bool send_message_move(int msqid, pid_t pid, int direction);

bool send_message_place_bomb(int msqid, pid_t pid);

bool receive_message_response(int msqid, pid_t pid);
