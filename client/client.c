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
sem_t sem_display;



/* ------------------------------------------------------------------------- */
/*                                 Functions                                 */
/* ------------------------------------------------------------------------- */

int main(void)
{
    struct message_server_response response;
    pthread_t th_game_state, th_game_end;

    pid = getpid();

    // Enter the game code
    int game_code;
    printf("Enter the game code : ");
    scanf("%d", &game_code);

    // Retrieve the main message queue ID & tell the server we want to connect
    int msqid = msgget(ftok(TOKEN_PATH_NAME, game_code), 0);
    send_message_connection(msqid, getpid());

    // Wait for the server response
    msgrcv(msqid, &response, sizeof(response.mcontent), pid, 0);

    if (!response.mcontent.success) {
        printf("Connection refused.\n");
        return EXIT_FAILURE;
    }
    printf("Connected! Waiting for the game to start...\n");

    // Retrieve the client's message queue ID
    client_msqid = msgget(ftok(TOKEN_PATH_NAME, pid), 0);

    // Start threads that will handle the server's messages in the client's message queue
    pthread_create(&th_game_state, NULL, thread_message_game_state, NULL);
    pthread_create(&th_game_end, NULL, thread_message_game_end, NULL);
    pthread_join(th_game_state, NULL);
    pthread_join(th_game_end, NULL);

    return 0;
}



void* thread_display(void* arg)
{
    int x, y;

    while (game_running) {
        // Wait for the game state to be received
        sem_wait(&sem_display);

        clear();

        // Display the map
        for (int i = 0; i < MAP_HEIGHT; i++) {
            for (int n = 0; n < 2; n++) { // Print 2 times each line
                for (int j = 0; j < MAP_WIDTH; j++) {
                    switch (game.map[j][i]) {
                    case MAP_TILE_WALL:           printw("####"); break;
                    case MAP_TILE_BREAKABLE_WALL: printw("////"); break;
                    case MAP_TILE_EMPTY:          printw("    "); break;
                    }
                }
                move(i*2 + n + 1, 0);
            }
        }

        // Display bombs
        for (int i = 0; i < game.player_count; i++) {
            if (!game.players[i].alive || !game.players[i].bomb.active)
                continue;

            if (game.players[i].bomb.exploded) {
                // TODO: Display the explosion fire

            }
            else {
                // Display the bomb as planted
                x = game.players[i].bomb.coords.x;
                y = game.players[i].bomb.coords.y;
                move(y*2, x*4);
                printw(" oo ");
                move(y*2 + 1, x*4);
                printw(" oo ");
            }
        }

        // Display players
        for (int i = 0; i < game.player_count; i++) {
            if (!game.players[i].alive)
                continue;

            x = game.players[i].coords.x;
            y = game.players[i].coords.y;
            move(y*2, x*4);
            for (int n = 0; n < 4; n++)
                addch((char)(i + 1 + 48));
            move(y*2 + 1, x*4);
            for (int n = 0; n < 4; n++)
                addch((char)(i + 1 + 48));
        }

        refresh();
    }

    pthread_exit(NULL);
}



void* thread_inputs(void* arg)
{
    while (game_running) {
        // Get the input
        int c = getch();

        // Send the input to the server if it is a valid one
        switch (c) {
        case CONTROL_KEY_UP: send_message_move(client_msqid, DIRECTION_UP); break;
        case CONTROL_KEY_DOWN: send_message_move(client_msqid, DIRECTION_DOWN); break;
        case CONTROL_KEY_LEFT: send_message_move(client_msqid, DIRECTION_LEFT); break;
        case CONTROL_KEY_RIGHT: send_message_move(client_msqid, DIRECTION_RIGHT); break;
        case CONTROL_KEY_PLACE_BOMB: send_message_place_bomb(client_msqid); break;
        default: break;
        }
    }

    pthread_exit(NULL);
}



void* thread_message_game_state(void* arg)
{
    struct message_server_game_state msg_game_state;

    // Initialize the semaphore used to display the game state
    // Display thread is asked to display the map every time the game state is received
    sem_init(&sem_display, 0, 0);

    do {
        // Receive game state message
        msgrcv(client_msqid, &msg_game_state, sizeof(msg_game_state.mcontent), MESSAGE_SERVER_GAME_STATE_TYPE, 0);

        // Store the game state in the global variable
        game = msg_game_state.mcontent.game_state;

        // Ask the display thread to display the game state
        sem_post(&sem_display);

        // If the game has not started yet, start the display thread
        if (!game_running) {
            game_running = true;
            printf("Game started!\n");

            // Initialize the ncurses window & disable echoing of typed characters
            initscr();
            noecho();

            // Start threads that will handle the inputs & displaying the game state
            pthread_t th_display, th_inputs;
            pthread_create(&th_display, NULL, thread_display, NULL);
            pthread_create(&th_inputs, NULL, thread_inputs, NULL);
        }
    } while (game_running);

    // Game has ended, close the ncurses window
    endwin();

    // Destroy the display semaphore
    sem_destroy(&sem_display);

    pthread_exit(NULL);
}



void* thread_message_game_end(void* arg)
{
    struct message_server_game_end msg_game_end;

    // Receive game end message
    msgrcv(client_msqid, &msg_game_end, sizeof(msg_game_end.mcontent), MESSAGE_SERVER_GAME_END_TYPE, 0);

    // Display the winner
    printf("Game ended. Winner : Player %d. Congrats!\n", msg_game_end.mcontent.winner + 1);

    game_running = false;

    pthread_exit(NULL);
}
