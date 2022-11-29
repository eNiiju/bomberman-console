/**
 * Main client file.
 * @file client.c
 * @date 2022-11-16
*/

#include "client.h"

/* ------------------------------------------------------------------------- */
/*                             Global variables                              */
/* ------------------------------------------------------------------------- */

pid_t pid;
bool server_response;

pthread_cond_t cond_server_response = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mut_server_response = PTHREAD_MUTEX_INITIALIZER;



/* ------------------------------------------------------------------------- */
/*                                 Functions                                 */
/* ------------------------------------------------------------------------- */

int main(void)
{
    pid = getpid();

    // Start the thread that will handle the server's messages
    pthread_t th_messages;
    pthread_create(&th_messages, NULL, thread_handle_messages, NULL);

    // Enter the game code
    int game_code;
    printf("Enter the game code : ");
    scanf("%d", &game_code);

    // Tell the server we want to connect
    int msqid = msgget(ftok(TOKEN_PATH_NAME, game_code), 0);
    send_message_connection(msqid, getpid());

    // Wait for the server response
    pthread_cond_wait(&cond_server_response, &mut_server_response);

    if (!server_response) {
        printf("Connection refused.\n");
        return EXIT_FAILURE;
    }
    else {
        printf("Connected.\n");
    }

    return 0;
}

void* thread_handle_messages(void* arg)
{
    int msqid = (int)arg;
    struct message_server msg_server;

    while (true) {
        msgrcv(msqid, &msg_server, sizeof(msg_server.mcontent), pid, 0);

        // Handle the message depending on its type
        switch (msg_server.mcontent.type) {
        case MESSAGE_TYPE_SERVER_GAME_STATE:
            break;
        case MESSAGE_TYPE_SERVER_GAME_END:
            break;
        case MESSAGE_TYPE_SERVER_RESPONSE:
            server_response = msg_server.mcontent.success;
            pthread_cond_signal(&cond_server_response);
            break;
        }
    }
}
