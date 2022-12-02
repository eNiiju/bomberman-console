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



bool send_connection_response(int main_msqid, bool success, pid_t pid_client)
{
    struct message_server_response response = {
        .mtype = pid_client,
        .mcontent = {
            .success = success
        }
    };
    return msgsnd(main_msqid, &response, sizeof(response.mcontent), 0) != -1;
}
