/**
 * Functions handling message queues.
 * @file message.c
 * @date 2022-11-23
*/

#include "message.h"

/* ------------------------------------------------------------------------- */
/*                                 Functions                                 */
/* ------------------------------------------------------------------------- */

bool send_message_connection(int msqid, pid_t pid)
{
    struct message_connection msg_connection = {
        .mtype = MESSAGE_TYPE_CONNECTION,
        .mcontent = {
            .pid = pid
        }
    };
    return msgsnd(msqid, &msg_connection, sizeof(msg_connection.mcontent), 0) != -1;
}



bool send_message_disconnection(int msqid, pid_t pid)
{
    struct message_disconnection msg_disconnection = {
        .mtype = MESSAGE_TYPE_DISCONNECTION,
        .mcontent = {
            .pid = pid
        }
    };
    return msgsnd(msqid, &msg_disconnection, sizeof(msg_disconnection.mcontent), 0) != -1;
}



bool send_message_move(int msqid, pid_t pid, int direction)
{
    struct message_move msg_move = {
        .mtype = MESSAGE_TYPE_MOVE,
        .mcontent = {
            .pid = pid,
            .direction = direction
        }
    };
    return msgsnd(msqid, &msg_move, sizeof(msg_move.mcontent), 0) != -1;
}



bool send_message_place_bomb(int msqid, pid_t pid)
{
    struct message_place_bomb msg_place_bomb = {
        .mtype = MESSAGE_TYPE_PLACE_BOMB,
        .mcontent = {
            .pid = pid
        }
    };
    return msgsnd(msqid, &msg_place_bomb, sizeof(msg_place_bomb.mcontent), 0) != -1;
}



bool receive_message_response(int msqid, pid_t pid)
{
    struct message_response msg_response;
    msgrcv(msqid, &msg_response, sizeof(msg_response.mcontent), pid, 0);
    return msg_response.mcontent.success;
}
