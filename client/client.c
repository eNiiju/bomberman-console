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
int client_msqid;
bool game_running = false;
struct game game;



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

    // Retrieve the main message queue ID & tell the server we want to connect
    int msqid = msgget(ftok(TOKEN_PATH_NAME, game_code), 0);
    send_message_connection(msqid, getpid());

    // Wait for the server response
    struct message_server_response response;
    msgrcv(msqid, &response, sizeof(response.mcontent), pid, 0);

    if (!response.mcontent.success) {
        printf("Connection refused.\n");
        return EXIT_FAILURE;
    }

    printf("Connected!\n");

    // Retrieve the client's message queue ID
    client_msqid = msgget(ftok(TOKEN_PATH_NAME, pid), 0);

    // Start threads that will handle the server's messages in the client's message queue
    pthread_t th_game_state, th_game_end;
    pthread_create(&th_game_state, NULL, thread_message_game_state, NULL);
    pthread_create(&th_game_end, NULL, thread_message_game_end, NULL);
    pthread_join(th_game_state, NULL);
    pthread_join(th_game_end, NULL);

    return 0;
}



void* thread_display(void* arg)
{
    while (game_running) {
        // Display game state
        // TODO : Display game state
    }

    pthread_exit(NULL);
}



void* thread_inputs(void* arg)
{
    while (game_running) {
        int c = getch();

        switch (c) {
        case _KEY_UP: send_message_move(client_msqid, DIRECTION_UP); break;
        case _KEY_DOWN: send_message_move(client_msqid, DIRECTION_DOWN); break;
        case _KEY_LEFT: send_message_move(client_msqid, DIRECTION_LEFT); break;
        case _KEY_RIGHT: send_message_move(client_msqid, DIRECTION_RIGHT); break;
        case _KEY_PLACE_BOMB: send_message_place_bomb(client_msqid); break;
        default: break;
        }
    }

    pthread_exit(NULL);
}



void* thread_message_game_state(void* arg)
{
    struct message_server_game_state msg_game_state;
    while (true) {
        // Receive game state message
        msgrcv(client_msqid, &msg_game_state, sizeof(msg_game_state.mcontent), MESSAGE_SERVER_GAME_STATE_TYPE, 0);

        // If the game has not started yet, start the display thread
        if (!game_running) {
            game_running = true;
            printf("Game started!\n");
            initscr(); // Initialise la structure WINDOW et autres paramètres de ncurses
            pthread_t th_display, th_inputs;
            pthread_create(&th_display, NULL, thread_display, NULL);
            pthread_create(&th_inputs, NULL, thread_inputs, NULL);
        }

        // Store the game state in the global variable
        game = msg_game_state.mcontent.game_state;
    }

    pthread_exit(NULL);
}



void* thread_message_game_end(void* arg)
{
    struct message_server_game_end msg_game_end;

    // Receive game end message
    msgrcv(client_msqid, &msg_game_end, sizeof(msg_game_end.mcontent), MESSAGE_SERVER_GAME_END_TYPE, 0);

    // TODO : Handle game end
    game_running = false;

    pthread_exit(NULL);
}
