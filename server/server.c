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

    // Start thread that will handle players' messages in main message queue (connections)
    pthread_t th_main_msq;
    pthread_create(&th_main_msq, NULL, thread_main_message_queue, NULL);
    printf("Server is listening with code %d\n\n", game.game_code);

    // Start the game when the mutex is unlocked
    pthread_mutex_lock(&mut_start_game);
    pthread_t th_game;
    pthread_create(&th_game, NULL, thread_game, NULL);
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

    // Create main message queue
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
    pid_t pid_client = game.players[player_number].pid_client;

    // Create player message queue with the client ID as projet ID when generating the token
    create_message_queue(pid_client);

    // Send response to client
    send_connection_response(game.msqid, true, pid_client);

    printf("Player %ld joined the game.\n", player_number);

    // Wait for client's messages
    pthread_t th_msg_move, th_msg_place_bomb;
    pthread_create(&th_msg_move, NULL, thread_player_message_move, (void*)player_number);
    pthread_create(&th_msg_place_bomb, NULL, thread_player_message_place_bomb, (void*)player_number);
    pthread_join(th_msg_move, NULL);
    pthread_join(th_msg_place_bomb, NULL);

    pthread_exit(0);
}



void* thread_game(void* arg)
{
    // Retrieve map data
    FILE* map_file = fopen(strcat(PATH_MAPS, "bbtty_default"), "r");
    if (map_file == NULL) {
        perror("Error while opening map file");
        pthread_exit(1);
    }

    int line = 0, column = 0;
    char c;

    do {
        c = fgetc(map_file);
        
        if (c == '\n') {
            line++;
            continue;
        }

        game.map[line][column] = c;
    } while (c != EOF);


    printf("\nGame has started!\n\n");

    while (true) {
        // Send the game state to all clients
        for (int i = 0; i < game.player_count; i++) {
            int client_msqid = get_client_msqid(game.players[i].pid_client);
            send_game_state(client_msqid, game);
        }

        sleep(5);
    }

    pthread_exit(0);
}



bool create_player(pid_t pid_client)
{
    // If the player already exists, do nothing
    if (client_in_game(pid_client))
        return false;

    // Initialize the player
    game.players[game.player_count] = (struct player){
        .pid_client = pid_client,
        .coords = {0, 0},
        .alive = true,
        .bomb_amount = 1,
        .bomb_range = DEFAULT_BOMB_RANGE
    };

    // Create the player thread
    pthread_t thplayer;
    pthread_create(&thplayer, NULL, thread_player, (void*)game.player_count);

    game.player_count++;
    return true;
}



bool client_in_game(pid_t pid_client)
{
    for (int i = 0; i < game.player_count; i++)
        if (game.players[i].pid_client == pid_client)
            return true;
    return false;
}



void* thread_main_message_queue(void* arg)
{
    struct message_client_connection msg;
    while (true) {
        // Receive a connection message from a client
        msgrcv(game.msqid, &msg, sizeof(msg.mcontent), MESSAGE_CLIENT_CONNECTION_TYPE, 0);
        pid_t pid_client = msg.mcontent.pid_client;

        if (game.player_count < MAX_PLAYERS) {
            bool created = create_player(pid_client);
            send_connection_response(game.msqid, created, pid_client);

            // If the players is now full, unlock the mutex to start the game
            if (game.player_count == MAX_PLAYERS)
                pthread_mutex_unlock(&mut_start_game);
        }
        else
            send_connection_response(game.msqid, false, pid_client);
    }

    pthread_exit(0);
}



void* thread_player_message_move(void* arg)
{
    long player_number = (long)arg;

    // Retrieve client message queue ID
    int client_msqid = get_client_msqid(game.players[player_number].pid_client);

    struct message_client_move msg_move;
    while (true) {
        // Receive move message
        msgrcv(client_msqid, &msg_move, sizeof(msg_move.mcontent), MESSAGE_CLIENT_MOVE_TYPE, 0);

        // TODO
    }

    pthread_exit(0);
}



void* thread_player_message_place_bomb(void* arg)
{
    long player_number = (long)arg;

    // Retrieve client message queue ID
    int client_msqid = get_client_msqid(game.players[player_number].pid_client);

    struct message_client_place_bomb msg_place_bomb;
    while (true) {
        // Receive move message
        msgrcv(client_msqid, &msg_place_bomb, 0, MESSAGE_CLIENT_PLACE_BOMB_TYPE, 0);

        // TODO
    }

    pthread_exit(0);
}
