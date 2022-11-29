/**
 * Main server file.
 * @file server.c
 * @date 2022-11-16
*/

#include "server.h"

/* ------------------------------------------------------------------------- */
/*                             Global variables                              */
/* ------------------------------------------------------------------------- */

struct game game = {
    .ended = false,
    .winner = -1,
    .player_count = 0
};

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
    printf("Server is listening with code %d\n", game.game_code);

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
    game.game_code = rand() % 10000;

    // Create message queue
    game.msqid = create_message_queue(game.game_code);
    if (game.msqid == -1) {
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

    // Initialize the player
    game.players[game.player_count].client_pid = client_pid;
    game.players[game.player_count].coords.x = 0;
    game.players[game.player_count].coords.y = 0;
    game.players[game.player_count].alive = true;
    game.players[game.player_count].bomb_amount = 1;
    game.players[game.player_count].bomb_range = DEFAULT_BOMB_RANGE;

    // Create the player thread
    pthread_t thplayer;
    pthread_create(&thplayer, NULL, thread_player, (void*)game.player_count);

    game.player_count++;
    return true;
}



bool client_in_game(pid_t client_pid)
{
    for (int i = 0; i < game.player_count; i++)
        if (game.players[i].client_pid == client_pid)
            return true;
    return false;
}



void* thread_message_connection(void* arg)
{
    struct message_connection msg_connection;
    while (true) {
        // Receive a connection message from a client
        msgrcv(game.msqid, &msg_connection, sizeof(msg_connection.mcontent), MESSAGE_TYPE_CLIENT_CONNECTION, 0);

        if (game.player_count < MAX_PLAYERS) {
            bool success = create_player(msg_connection.mcontent.pid);
            send_response(game.msqid, success, msg_connection.mcontent.pid);

            if (success)
                printf("Player %d joined the game.\n", msg_connection.mcontent.pid);
            
            // If the players is now full, unlock the mutex to start the game
            if (game.player_count == MAX_PLAYERS)
                pthread_mutex_unlock(&mut_start_game);
        }
        else
            send_response(game.msqid, false, msg_connection.mcontent.pid);
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
