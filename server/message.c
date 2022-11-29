/**
 * Functions handling message queues.
 * @file message.c
 * @date 2022-11-23
*/

#include "message.h"

/* ------------------------------------------------------------------------- */
/*                                 Functions                                 */
/* ------------------------------------------------------------------------- */

int create_message_queue(int game_code)
{
    return msgget(ftok(TOKEN_PATH_NAME, game_code), IPC_CREAT | IPC_EXCL | 0666);
}



bool send_response(int msqid, bool success, pid_t pid_client)
{
    struct message_response response = {
        .mtype = pid_client,
        .mcontent = {
            .success = success,
            .pid = getpid()
        }
    };
    return msgsnd(msqid, &response, sizeof(response.mcontent), 0) != -1;
}
