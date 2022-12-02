/**
 * Functions handling message queues.
 * @file message.c
 * @date 2022-11-23
*/

#include "message.h"

/* ------------------------------------------------------------------------- */
/*                                 Functions                                 */
/* ------------------------------------------------------------------------- */

bool send_message_connection(int main_msqid, pid_t pid)
{
    struct message_client_connection msg = {
        .mtype = MESSAGE_CLIENT_CONNECTION_TYPE,
        .mcontent = {
            .pid_client = pid
        }
    };
    return msgsnd(main_msqid, &msg, sizeof(msg.mcontent), 0) != -1;
}



bool send_message_disconnection(int client_msqid)
{
    struct message_client_disconnection msg = {
        .mtype = MESSAGE_CLIENT_DISCONNECTION_TYPE
    };
    return msgsnd(client_msqid, &msg, 0, 0) != -1;
}



bool send_message_move(int client_msqid, int direction)
{
    struct message_client_move msg = {
        .mtype = MESSAGE_CLIENT_MOVE_TYPE,
        .mcontent = {
            .direction = direction
        }
    };
    return msgsnd(client_msqid, &msg, sizeof(msg.mcontent), 0) != -1;
}



bool send_message_place_bomb(int client_msqid)
{
    struct message_client_place_bomb msg = {
        .mtype = MESSAGE_CLIENT_PLACE_BOMB_TYPE,
    };
    return msgsnd(client_msqid, &msg, 0, 0) != -1;
}
