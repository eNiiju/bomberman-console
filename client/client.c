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



/* ------------------------------------------------------------------------- */
/*                                 Functions                                 */
/* ------------------------------------------------------------------------- */

int main(void)
{
    pid = getpid();

    // Enter the game code
    int game_code;
    printf("Enter the game code : ");
    scanf("%d", &game_code);

    // Tell the server we want to connect
    int msqid = msgget(ftok(TOKEN_PATH_NAME, game_code), 0);
    send_message_connection(msqid, getpid());

    // Wait for the server response
    bool response = receive_message_response(msqid, pid);

    if (!response) {
        printf("Connection refused.\n");
        return EXIT_FAILURE;
    }
    else {
        printf("Connected.\n");
    }

    return 0;
}
