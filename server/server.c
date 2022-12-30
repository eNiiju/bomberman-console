/**
 * Main server file.
 * @file server.c
 * @date 2022-11-16
*/

#include "server.h"

/* ------------------------------------------------------------------------- */
/*                             Global variables                              */
/* ------------------------------------------------------------------------- */

struct game game;
pthread_mutex_t mut_start_game;
pthread_mutex_t mut_player_move[MAX_PLAYERS];
pthread_mutex_t mut_player_place_bomb[MAX_PLAYERS];



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
    // Initialize mutexes
    pthread_mutex_init(&mut_start_game, NULL);
    for (int i = 0; i < MAX_PLAYERS; i++) {
        pthread_mutex_init(&mut_player_move[i], NULL);
        pthread_mutex_init(&mut_player_place_bomb[i], NULL);
    }

    // Prevent the game from starting
    pthread_mutex_lock(&mut_start_game);

    // Initialize game global variable
    game = (struct game){
        .ended = false,
        .winner = -1,
        .player_count = 0
    };

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

    pthread_exit(NULL);
}



void* thread_game(void* arg)
{
    // TODO : Function to retrieve map data
    // Retrieve map data
    char path_to_map[MAX_LENGTH_PATH_TO_MAP] = PATH_MAPS;
    strcat(path_to_map, "bbtty_default");
    FILE* map_file = fopen(path_to_map, "r");

    if (map_file == NULL) {
        perror("Error while opening map file");
        pthread_exit(NULL);
    }

    // Retrieve all lines in the map file
    char lines[MAP_HEIGHT][MAP_WIDTH + 2];
    int i = 0;
    while (fgets(lines[i], MAP_WIDTH + 2, map_file) != NULL) {
        // Remove the newline character from the end of the string
        size_t len = strlen(lines[i]);
        if (len > 0 && lines[i][len - 1] == '\n')
            lines[i][len - 1] = '\0';
        i++;
    }

    // Store the map data in the game structure
    for (int line = 0; line < MAP_HEIGHT; line++) {
        for (int column = 0; column < MAP_WIDTH; column++) {
            char c = lines[line][column];
            if (c == '1' || c == '2' || c == '3' || c == '4') {
                // If it's a player, set it's coordinates and set the case to empty
                // (will be replaced by the player on the client display)
                game.players[c - '1'].coords = (struct coordinates){column, line};
                game.map[line][column] = MAP_CASE_EMPTY;
            }
            else
                game.map[line][column] = c;
        }
    }    

    fclose(map_file);

    printf("\nGame has started!\n\n");

    while (!game.ended) {
        // Send the game state to all clients
        for (int i = 0; i < game.player_count; i++) {
            int client_msqid = get_client_msqid(game.players[i].pid_client);
            send_game_state(client_msqid, game);
        }

        usleep(REFRESH_RATE_MS * 1000);
    }

    printf("\nGame has ended.\n");

    pthread_exit(NULL);
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
    while (!game.ended) {
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

    pthread_exit(NULL);
}



void* thread_player_message_move(void* arg)
{
    long player_number = (long)arg;

    // Retrieve client message queue ID
    int client_msqid = get_client_msqid(game.players[player_number].pid_client);

    struct message_client_move msg_move;
    while (!game.ended) {
        // Receive move message
        msgrcv(client_msqid, &msg_move, sizeof(msg_move.mcontent), MESSAGE_CLIENT_MOVE_TYPE, 0);

        // Can the player move here?
        bool can_move = player_can_move(player_number, msg_move.mcontent.direction);

        // Actually move the player
        if (can_move)
            move_player(player_number, msg_move.mcontent.direction);
    }

    pthread_exit(NULL);
}



void* thread_player_message_place_bomb(void* arg)
{
    long player_number = (long)arg;

    // Retrieve client message queue ID
    int client_msqid = get_client_msqid(game.players[player_number].pid_client);

    struct message_client_place_bomb msg_place_bomb;
    while (!game.ended) {
        // Receive move message
        msgrcv(client_msqid, &msg_place_bomb, 0, MESSAGE_CLIENT_PLACE_BOMB_TYPE, 0);

        // Can the player place a bomb?
        bool can_place_bomb = player_can_place_bomb(player_number);

        // Actually place the bomb
        if (can_place_bomb)
            place_bomb(player_number);
    }

    pthread_exit(NULL);
}



bool player_can_move(long player_number, int direction)
{
    return true;
}



bool player_can_place_bomb(long player_number)
{
    return true;
}



void* thread_wait_player_move_cooldown(void* arg)
{
    long player_number = (long)arg;

    // Wait for the cooldown to end
    usleep(PLAYER_MOVE_COOLDOWN_MS * 1000);

    // Unlock the mutex for that player
    pthread_mutex_unlock(&mut_player_move[player_number]);

    pthread_exit(NULL);
}



void move_player(long player_number, int direction)
{
    // Lock the mutex for that player
    pthread_mutex_lock(&mut_player_move[player_number]);

    // Move the player on the map
    switch (direction) {
        case DIRECTION_UP:
            game.players[player_number].coords.y--;
            break;
        case DIRECTION_DOWN:
            game.players[player_number].coords.y++;
            break;
        case DIRECTION_LEFT:
            game.players[player_number].coords.x--;
            break;
        case DIRECTION_RIGHT:
            game.players[player_number].coords.x++;
            break;
    }

    // Create a thread to wait for the cooldown to end
    pthread_t th_wait_cooldown;
    pthread_create(&th_wait_cooldown, NULL, thread_wait_player_move_cooldown, (void*)player_number);
}



void place_bomb(long player_number)
{
    // Lock the mutex for that player
    pthread_mutex_lock(&mut_player_place_bomb[player_number]);

    // Retrieve the player's coordinates
    int x = game.players[player_number].coords.x;
    int y = game.players[player_number].coords.y;

    // Place the bomb
    game.map[y][x] = MAP_CASE_BOMB;
    
    // Create a thread to wait for the bomb to explode
    // TODO : unlock bomb mutex in thread after cooldown
}
