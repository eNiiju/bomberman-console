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
int player_number;



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
    printf("Hint: Press 'r' to refresh the display if there's a display glitch.\n");

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
    while (game_running) {
        // Wait for the game state to be received
        sem_wait(&sem_display);

        if (!game_running)
            break;

        move(0, 0);

        display_map(&game);
        display_bombs(&game);
        display_players(&game);
        display_informations(&game, player_number);

        refresh();
    }

    pthread_exit(NULL);
}



void* thread_inputs(void* arg)
{
    // Set the timeout for the getch() function
    timeout(1000);

    while (game_running) {
        // Get the input
        int c = getch();

        if (!game_running)
            break;

        // Send the input to the server if it is a valid one
        switch (c) {
        case CONTROL_KEY_UP: send_message_move(client_msqid, DIRECTION_UP); break;
        case CONTROL_KEY_DOWN: send_message_move(client_msqid, DIRECTION_DOWN); break;
        case CONTROL_KEY_LEFT: send_message_move(client_msqid, DIRECTION_LEFT); break;
        case CONTROL_KEY_RIGHT: send_message_move(client_msqid, DIRECTION_RIGHT); break;
        case CONTROL_KEY_PLACE_BOMB: send_message_place_bomb(client_msqid); break;
        case CONTROL_KEY_REFRESH: clear(); sem_post(&sem_display); break;
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

        if (msg_game_state.mcontent.stop)
            break;

        // Store the game state in the global variable
        game = msg_game_state.mcontent.game_state;

        // Ask the display thread to display the game state
        sem_post(&sem_display);

        // If the game has not started yet, start the display thread
        if (!game_running) {
            game_running = true;
            printf("Game started!\n");

            // Retrieve the player number
            int i = 0;
            while (game.players[i].pid_client != pid)
                i++;
            player_number = i;

            // Initialize the ncurses window & disable echoing of typed characters
            initscr();
            noecho();

            // Start threads that will handle the inputs & displaying the game state
            pthread_t th_display, th_inputs;
            pthread_create(&th_display, NULL, thread_display, NULL);
            pthread_create(&th_inputs, NULL, thread_inputs, NULL);
        }
    } while (game_running);

    pthread_exit(NULL);
}



void* thread_message_game_end(void* arg)
{
    struct message_server_game_end msg_game_end;

    // Receive game end message
    msgrcv(client_msqid, &msg_game_end, sizeof(msg_game_end.mcontent), MESSAGE_SERVER_GAME_END_TYPE, 0);

    game_running = false;
    sleep(1);

    // Game has ended, close the ncurses window
    clear();
    refresh();
    endwin();

    // End the display thread
    sem_post(&sem_display);

    // Destroy the display semaphore
    sem_destroy(&sem_display);

    // Display the winner
    printf("Game ended. Winner : Player %d. Congrats!\n", msg_game_end.mcontent.winner + 1);

    pthread_exit(NULL);
}
