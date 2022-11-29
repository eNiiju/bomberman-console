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
pthread_mutex_t mut_start_game = PTHREAD_MUTEX_INITIALIZER;



/* ------------------------------------------------------------------------- */
/*                                 Functions                                 */
/* ------------------------------------------------------------------------- */

int main(void)
{
    if (!setup()) {
        printf("Server setup failed.\n");
        return EXIT_FAILURE;
    }

    // Start threads that will handle players' messages
    pthread_t th_messages[4];
    pthread_create(&th_messages[0], NULL, thread_message_connection, NULL);
    pthread_create(&th_messages[1], NULL, thread_message_disconnection, NULL);
    pthread_create(&th_messages[2], NULL, thread_message_move, NULL);
    pthread_create(&th_messages[3], NULL, thread_message_place_bomb, NULL);
    printf("Server is listening with code %d\n", game_code);

    // Start the game when the mutex is unlocked
    pthread_mutex_lock(&mut_start_game);
    pthread_t th_game;
    pthread_create(&th_game, NULL, thread_game, NULL);
    printf("Game started!\n");
    pthread_join(th_game, NULL);

    return 0;
}



bool setup(void)
{
    // Prevent the game from starting
    pthread_mutex_lock(&mut_start_game);

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



void* thread_message_connection(void* arg)
{
    struct message_connection msg_connection;
    while (true) {
        // Receive a connection message from a client
        msgrcv(msqid, &msg_connection, sizeof(msg_connection.mcontent), MESSAGE_TYPE_CONNECTION, 0);

        if (nb_players < MAX_PLAYERS) {
            bool success = create_player(msg_connection.mcontent.pid);
            send_response(msqid, success, msg_connection.mcontent.pid);

            if (success)
                printf("Player %d joined the game.\n", msg_connection.mcontent.pid);
            
            // If the players is now full, unlock the mutex to start the game
            if (nb_players == MAX_PLAYERS)
                pthread_mutex_unlock(&mut_start_game);
        }
        else
            send_response(msqid, false, msg_connection.mcontent.pid);
    }
}



void* thread_message_disconnection(void* arg)
{

}



void* thread_message_move(void* arg)
{
    
}



void* thread_message_place_bomb(void* arg)
{
    
}
