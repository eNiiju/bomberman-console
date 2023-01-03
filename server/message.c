/**
 * Functions handling message queues.
 * @file message.c
 * @date 2022-11-23
*/

#include "message.h"

/* ------------------------------------------------------------------------- */
/*                                 Functions                                 */
/* ------------------------------------------------------------------------- */

int create_message_queue(int project_id)
{
    return msgget(ftok(TOKEN_PATH_NAME, project_id), IPC_CREAT | IPC_EXCL | 0666);
}



int get_client_msqid(int pid_client)
{
    return msgget(ftok(TOKEN_PATH_NAME, pid_client), 0);
}



bool send_connection_response(int main_msqid, bool success, pid_t pid_client)
{
    struct message_server_response msg = {
        .mtype = pid_client,
        .mcontent = {
            .success = success
        }
    };
    return msgsnd(main_msqid, &msg, sizeof(msg.mcontent), 0) != -1;
}



bool send_response(pid_t client_msqid, bool success)
{
    struct message_server_response msg = {
        .mtype = MESSAGE_SERVER_RESPONSE_TYPE,
        .mcontent = {
            .success = success
        }
    };
    return msgsnd(client_msqid, &msg, sizeof(msg.mcontent), 0) != -1;
}



bool send_game_state(pid_t client_msqid, struct game game_state, bool stop)
{
struct message_server_game_state msg = {
        .mtype = MESSAGE_SERVER_GAME_STATE_TYPE,
        .mcontent = {
            .stop = stop,
            .game_state = game_state
        }
    };
    return msgsnd(client_msqid, &msg, sizeof(msg.mcontent), 0) != -1;
}



bool send_game_end(pid_t client_msqid, int winner)
{
    struct message_server_game_end msg = {
        .mtype = MESSAGE_SERVER_GAME_END_TYPE,
        .mcontent = {
            .winner = winner
        }
    };
    return msgsnd(client_msqid, &msg, sizeof(msg.mcontent), 0) != -1;
}
