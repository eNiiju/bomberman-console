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
pthread_mutex_t mut_create_player; // Prevents multiple players from being created with the same number



/* ------------------------------------------------------------------------- */
/*                                 Functions                                 */
/* ------------------------------------------------------------------------- */

int main(int argc, char* argv[])
{
    pthread_t th_main_msq, th_game;

    // Check if the map file is provided
    if (argc < 2) {
        printf("Usage: %s <path_to_map_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (!setup(argv[1])) {
        printf("Server setup failed.\n");
        return EXIT_FAILURE;
    }

    // Start thread that will handle players' messages in main message queue (connections)
    pthread_create(&th_main_msq, NULL, thread_main_message_queue, NULL);
    printf("Server is listening with code %d\n\n", game.game_code);

    // Start the game when the mutex is unlocked
    pthread_mutex_lock(&mut_start_game);
    pthread_create(&th_game, NULL, thread_game, NULL);
    pthread_join(th_game, NULL);

    return 0;
}



bool setup(char* path_to_map_file)
{
    // Initialize mutexes
    pthread_mutex_init(&mut_start_game, NULL);
    pthread_mutex_init(&mut_create_player, NULL);

    // Prevent the game from starting
    pthread_mutex_lock(&mut_start_game);

    // Initialize random seed
    srand(time(NULL));

    // Initialize game global variable
    game = (struct game){
        .game_code = rand() % 10000,
        .ended = false,
        .winner = -1,
        .player_count = 0
    };
    game.msqid = create_message_queue(game.game_code);

    if (strlen(path_to_map_file) > MAX_LENGTH_PATH_TO_MAP) {
        printf("Path to map file is too long. (Should be under %d characters)\n", MAX_LENGTH_PATH_TO_MAP);
        return false;
    }
    strcpy(game.path_to_map_file, path_to_map_file);

    if (game.msqid == -1)
        perror("Error while creating message queue.\n");

    return true;
}



void* thread_player(void* arg)
{
    int player_number = game.player_count++;
    pthread_mutex_unlock(&mut_create_player);

    pid_t pid_client = game.players[player_number].pid_client;
    pthread_t th_msg_move, th_msg_place_bomb;

    // Create player message queue with the client ID as projet ID when generating the token
    create_message_queue(pid_client);

    // Send response to client
    send_connection_response(game.msqid, true, pid_client);
    printf("Player %d joined the game.\n", player_number + 1);

    // Wait for client's messages
    pthread_create(&th_msg_move, NULL, thread_player_message_move, (void*)&player_number);
    pthread_create(&th_msg_place_bomb, NULL, thread_player_message_place_bomb, (void*)&player_number);
    pthread_join(th_msg_move, NULL);
    pthread_join(th_msg_place_bomb, NULL);

    pthread_exit(NULL);
}



void* thread_game(void* arg)
{
    retrieve_map_data(game.path_to_map_file);

    printf("\nGame has started!\n\n");

    while (!game.ended) {
        // Send the game state to all clients
        for (int i = 0; i < game.player_count; i++) {
            int client_msqid = get_client_msqid(game.players[i].pid_client);
            send_game_state(client_msqid, game);
        }

        usleep(REFRESH_RATE_MS * 1000);

        check_game_end();
    }

    printf("\nGame has ended.\n");

    pthread_exit(NULL);
}



void retrieve_map_data(char* path_to_map_file)
{
    FILE* map_file = fopen(path_to_map_file, "r");

    if (map_file == NULL) {
        perror("Error while opening map file");
        pthread_exit(NULL);
    }

    // Retrieve all lines in the map file
    int i = 0;
    char lines[MAP_HEIGHT][MAP_WIDTH + 2];
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
                game.players[c - '1'].coords = (struct coordinates){ column, line };
                game.map[line][column] = MAP_TILE_EMPTY;
            }
            else
                game.map[line][column] = c;
        }
    }

    fclose(map_file);
}



bool create_player(pid_t pid_client)
{
    pthread_t th_player;

    // If the player already exists, do nothing
    if (client_in_game(pid_client))
        return false;

    pthread_mutex_lock(&mut_create_player);

    // Initialize the player
    game.players[game.player_count] = (struct player){
        .pid_client = pid_client,
        .alive = true,
        .coords = {0, 0},
        .bomb_range = DEFAULT_BOMB_RANGE
    };

    // Create the player thread
    pthread_create(&th_player, NULL, thread_player, NULL);

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
            bool player_created = create_player(pid_client);
            send_connection_response(game.msqid, player_created, pid_client);

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
    int player_number = *(int*)arg;
    struct message_client_move msg_move;
    pthread_t th_move;

    // Retrieve client message queue ID
    int client_msqid = get_client_msqid(game.players[player_number].pid_client);

    while (!game.ended) {
        // Receive move message
        msgrcv(client_msqid, &msg_move, sizeof(msg_move.mcontent), MESSAGE_CLIENT_MOVE_TYPE, 0);

        // Can the player move here?
        bool can_move = player_can_move(player_number, msg_move.mcontent.direction);

        // Actually move the player
        if (can_move) {
            struct player_move pm = { player_number, msg_move.mcontent.direction };
            pthread_create(&th_move, NULL, thread_move_player, (void*)&pm);
        }
    }

    pthread_exit(NULL);
}



void* thread_player_message_place_bomb(void* arg)
{
    int player_number = *(int*)arg;
    struct message_client_place_bomb msg_place_bomb;
    pthread_t th_place_bomb;

    // Retrieve client message queue ID
    int client_msqid = get_client_msqid(game.players[player_number].pid_client);

    while (!game.ended) {
        // Receive move message
        msgrcv(client_msqid, &msg_place_bomb, 0, MESSAGE_CLIENT_PLACE_BOMB_TYPE, 0);

        // Can the player place a bomb?
        bool can_place_bomb = player_can_place_bomb(player_number);

        // Actually place the bomb
        if (can_place_bomb)
            pthread_create(&th_place_bomb, NULL, thread_place_bomb, (void*)&player_number);
    }

    pthread_exit(NULL);
}



bool player_can_move(int player_number, int direction)
{
    // TODO : reject if the player doesnt have its cooldown

    // Get the player coordinates
    struct coordinates coords = game.players[player_number].coords;

    // Get the coordinates of the case the player wants to move to
    switch (direction) {
    case DIRECTION_UP:    coords.y--; break;
    case DIRECTION_DOWN:  coords.y++; break;
    case DIRECTION_LEFT:  coords.x--; break;
    case DIRECTION_RIGHT: coords.x++; break;
    }

    // Check if there is a player on this case
    for (int i = 0; i < game.player_count; i++)
        if (i != player_number
            && game.players[i].alive
            && game.players[i].coords.x == coords.x
            && game.players[i].coords.y == coords.y)
            return false;

    // Check if there is a planted bomb on this case
    for (int i = 0; i < game.player_count; i++)
        if (game.players[i].bomb.active
            && !game.players[i].bomb.exploded
            && game.players[i].bomb.coords.x == coords.x
            && game.players[i].bomb.coords.y == coords.y)
            return false;

    // Check if the player can move to this case
    switch (game.map[coords.y][coords.x]) {
    case MAP_TILE_EMPTY:          return true;
    case MAP_TILE_WALL:           return false;
    case MAP_TILE_BREAKABLE_WALL: return false;
    default:                      return false;
    }
}



bool player_can_place_bomb(int player_number)
{
    // If the player already has a bomb on the map, he can't plant another one
    if (game.players[player_number].bomb.active)
        return false;
    return true;
}



void* thread_move_player(void* arg)
{
    struct player_move player_move = *(struct player_move*)arg;
    int player_number = player_move.player_number;
    int direction = player_move.direction;

    // Move the player on the map
    switch (direction) {
    case DIRECTION_UP:    game.players[player_number].coords.y--; break;
    case DIRECTION_DOWN:  game.players[player_number].coords.y++; break;
    case DIRECTION_LEFT:  game.players[player_number].coords.x--; break;
    case DIRECTION_RIGHT: game.players[player_number].coords.x++; break;
    }

    // Check if the player died by moving on a bomb's explosion
    if (check_player_death(player_number))
        game.players[player_number].alive = false;

    pthread_exit(NULL);
}



void* thread_place_bomb(void* arg)
{
    int player_number = *(int*)arg;

    // Retrieve the player's coordinates
    int x = game.players[player_number].coords.x;
    int y = game.players[player_number].coords.y;

    // Place the bomb
    game.players[player_number].bomb = (struct bomb){
        .active = true,
        .coords = { x, y },
        .range = game.players[player_number].bomb_range,
        .exploded = false
    };

    // Wait for the bomb to explode
    usleep(BOMB_EXPLOSION_TIMER_MS * 1000);

    // Apply the explosion to the map
    game.players[player_number].bomb.exploded = true;

    // TODO: Remove breakable walls that have been destroyed by the explosion


    // Check if the bomb killed a player
    for (int i = 0; i < game.player_count; i++)
        if (check_player_death(i))
            game.players[i].alive = false;

    // Wait a bit
    usleep(BOMB_FIRE_TIME_MS * 1000);

    // Remove the bomb from the map
    game.players[player_number].bomb.active = false;

    pthread_exit(NULL);
}



bool check_player_death(int player_number)
{
    // TODO : Check if the player is dead
    return false;
}



void check_game_end(void)
{
    // TODO : Check if the game is over
}
