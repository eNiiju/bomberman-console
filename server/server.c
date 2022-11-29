/**
 * Main server file.
 * @file server.c
 * @date 2022-11-16
*/

#include "server.h"

/* ------------------------------------------------------------------------- */
/*                             Global variables                              */
/* ------------------------------------------------------------------------- */

struct player players[MAX_PLAYERS];
long nb_players = 0;
int game_code;
int msqid;



/* ------------------------------------------------------------------------- */
/*                                 Functions                                 */
/* ------------------------------------------------------------------------- */

int main(void)
{
    if (!setup()) {
        printf("Server setup failed.\n");
        return EXIT_FAILURE;
    }

    // Wait for clients to connect
    printf("Server is listening with code %d\n", game_code);
    struct message_connection msg_connection;
    while (nb_players < MAX_PLAYERS) {
        // Receive a connection message from a client
        msgrcv(msqid, &msg_connection, sizeof(msg_connection.mcontent), MESSAGE_TYPE_CONNECTION, 0);

        // Create the player (if not already created)
        bool success = create_player(msg_connection.mcontent.pid);

        send_response(msqid, success, msg_connection.mcontent.pid);

        if (success)
            printf("Player %d joined the game.\n", msg_connection.mcontent.pid);
        else
            printf("Player %d already connected.\n", msg_connection.mcontent.pid);
    }

    // Start the game
    printf("Game started!\n");
    pthread_t thgame;
    pthread_create(&thgame, NULL, thread_game, NULL);
    pthread_join(thgame, NULL);

    return 0;
}



bool setup(void)
{
    // Generate game code
    srand(time(NULL));
    game_code = rand() % 10000;

    // Create message queue
    msqid = create_message_queue(game_code);
    if (msqid == -1) {
        perror("Error while creating message queue");
        return false;
    }

    return true;
}



void* thread_player(void* arg)
{
    long player_number = (long)arg;
    printf("Player %ld joined the game.\n", player_number);
}



void* thread_game(void* arg)
{

}



bool create_player(pid_t client_pid)
{
    // If the player already exists, do nothing
    if (client_in_game(client_pid))
        return false;

    pthread_t thplayer;

    // Initialize the player
    players[nb_players].client_pid = client_pid;
    players[nb_players].coords.x = 0;
    players[nb_players].coords.y = 0;
    players[nb_players].alive = true;
    players[nb_players].bomb_amount = 1;
    players[nb_players].bomb_range = DEFAULT_BOMB_RANGE;

    // Create the player thread
    pthread_create(&thplayer, NULL, thread_player, (void*)nb_players);

    nb_players++;
    return true;
}



bool client_in_game(pid_t client_pid)
{
    for (int i = 0; i < nb_players; i++)
        if (players[i].client_pid == client_pid)
            return true;
    return false;
}
