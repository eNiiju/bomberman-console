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
    if (argc < 3) {
        printf("Usage: %s <number_of_players> <path_to_map_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (!setup(argv[1], argv[2])) {
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

    // Game ended, clean up
    if (!clean_exit()) {
        printf("Clean exit failed.\n");
        return EXIT_FAILURE;
    }
    printf("Server is shutting down clean.\n");

    return 0;
}



bool setup(char* number_of_players, char* path_to_map_file)
{
    // Check arguments
    if (atoi(number_of_players) > MAX_PLAYERS || atoi(number_of_players) < 2) {
        printf("Wrong player number. (Should be between 2 and %d)\n", MAX_PLAYERS);
        return false;
    }

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
        .player_count = 0,
        .number_of_players = atoi(number_of_players)
    };
    game.msqid = create_message_queue(game.game_code);
    strcpy(game.path_to_map_file, path_to_map_file);
    retrieve_map_name(path_to_map_file);

    if (game.msqid == -1) {
        perror("Error while creating message queue");
        return false;
    }

    return true;
}


bool clean_exit()
{
    // Delete the main message queue
    msgctl(game.msqid, IPC_RMID, NULL);

    // Delete all players' message queues
    for (int i = 0; i < game.number_of_players; i++)
        msgctl(get_client_msqid(game.players[i].pid_client), IPC_RMID, NULL);

    // Destroy mutexes
    pthread_mutex_destroy(&mut_start_game);
    pthread_mutex_destroy(&mut_create_player);

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
    printf("Player %d joined the game\n", player_number + 1);

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

    sleep(3);

    printf("\nGame has started!\n");

    while (!game.ended) {
        // Send the game state to all clients
        for (int i = 0; i < game.number_of_players; i++) {
            int client_msqid = get_client_msqid(game.players[i].pid_client);
            send_game_state(client_msqid, game, false);
        }

        usleep(REFRESH_DELAY_MS * 1000);

        check_game_end();
    }

    printf("Game has ended.\n");

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
    char lines[MAX_MAP_HEIGHT][MAX_MAP_WIDTH + 2];
    while (fgets(lines[i], MAX_MAP_WIDTH + 2, map_file) != NULL) {
        // Remove the newline character from the end of the string
        size_t len = strlen(lines[i]);
        if (len > 0 && lines[i][len - 1] == '\n')
            lines[i][len - 1] = '\0';
        i++;
    }

    // Map height
    game.map_height = i;

    // Map width (take the first line and assume it's a rectangle)
    i = 0;
    while (i < MAX_MAP_WIDTH && lines[0][i] != '\0')
        i++;

    game.map_width = i;

    printf("Map size: %dx%d\n", game.map_width, game.map_height);

    // Store the map data in the game structure
    for (int line = 0; line < game.map_height; line++) {
        for (int column = 0; column < game.map_width; column++) {
            char c = lines[line][column];
            if (c > '0' && c <= '0' + MAX_PLAYERS) {
                // If it's a player, set it's coordinates and set the case to empty
                // (will be replaced by the player on the client display)
                // Only up to the number of players !
                int player_number = c - '1';
                if (player_number < game.number_of_players) {
                    game.players[player_number].coords = (struct coordinates){ column, line };
                    printf("Player %d will spawn at (%d, %d)\n", player_number + 1, column, line);
                }
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
        .bomb_range = DEFAULT_BOMB_RANGE,
        .can_move = true
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

        if (game.player_count < game.number_of_players) {
            bool player_created = create_player(pid_client);
            send_connection_response(game.msqid, player_created, pid_client);

            // If the players is now full, unlock the mutex to start the game
            if (game.player_count == game.number_of_players)
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
    if (!game.players[player_number].alive)
        return false;

    if (!game.players[player_number].can_move)
        return false;

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
    for (int i = 0; i < game.number_of_players; i++)
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
    if (check_player_death(player_number)) {
        game.players[player_number].alive = false;
        game.player_count--;
        printf("Player %d died\n", player_number + 1);
    }
    else {
        // Prevent the player from moving
        game.players[player_number].can_move = false;

        // Wait a bit (cooldown)
        usleep(PLAYER_MOVE_COOLDOWN_MS * 1000);

        // Allow the player to move again
        game.players[player_number].can_move = true;
    }

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

    printf("Player %d placed a bomb at (%d, %d)\n", player_number + 1, x, y);

    // Wait for the bomb to explode
    usleep(BOMB_EXPLOSION_TIMER_MS * 1000);

    // Apply the explosion to the map
    game.players[player_number].bomb.exploded = true;

    // Remove breakable walls that have been destroyed by the explosion
    int next_x, next_y;
    int up = 1, down = 1, left = 1, right = 1;

    for (int j = 1; j <= game.players[player_number].bomb.range; j++) {
        // Up
        next_x = x; next_y = y - j;
        if (up == j && next_y >= 0 && game.map[next_y][next_x] != MAP_TILE_WALL) {
            if (game.map[next_y][next_x] == MAP_TILE_BREAKABLE_WALL)
                game.map[next_y][next_x] = MAP_TILE_EMPTY;
            else up++;
        }

        // Down
        next_x = x; next_y = y + j;
        if (down == j && next_y < game.map_height && game.map[next_y][next_x] != MAP_TILE_WALL) {
            if (game.map[next_y][next_x] == MAP_TILE_BREAKABLE_WALL)
                game.map[next_y][next_x] = MAP_TILE_EMPTY;
            else down++;
        }

        // Left
        next_x = x - j; next_y = y;
        if (left == j && next_x >= 0 && game.map[next_y][next_x] != MAP_TILE_WALL) {
            if (game.map[next_y][next_x] == MAP_TILE_BREAKABLE_WALL)
                game.map[next_y][next_x] = MAP_TILE_EMPTY;
            else left++;
        }

        // Right
        next_x = x + j; next_y = y;
        if (right == j && next_x < game.map_width && game.map[next_y][next_x] != MAP_TILE_WALL) {
            if (game.map[next_y][next_x] == MAP_TILE_BREAKABLE_WALL)
                game.map[next_y][next_x] = MAP_TILE_EMPTY;
            else right++;
        }
    }

    // Check if the bomb killed a player
    for (int i = 0; i < game.number_of_players; i++)
        if (game.players[i].alive && check_player_death(i)) {
            game.players[i].alive = false;
            game.player_count--;
            printf("Player %d died\n", i + 1);
        }

    // Wait a bit
    usleep(BOMB_FIRE_TIME_MS * 1000);

    // Remove the bomb from the map
    game.players[player_number].bomb.active = false;

    pthread_exit(NULL);
}



bool check_player_death(int player_number)
{
    int player_x = game.players[player_number].coords.x;
    int player_y = game.players[player_number].coords.y;

    // For each bomb on the map
    for (int i = 0; i < game.number_of_players; i++) {
        if (game.players[i].alive && game.players[i].bomb.active && game.players[i].bomb.exploded) {
            int bomb_x = game.players[i].bomb.coords.x;
            int bomb_y = game.players[i].bomb.coords.y;

            // Check if the player is on the bomb's explosion
            if (player_x == bomb_x && player_y == bomb_y)
                return true;

            // Check if the player is on the bomb's explosion's range
            int next_x, next_y;
            int up = 1, down = 1, left = 1, right = 1;
            for (int j = 1; j <= game.players[i].bomb.range; j++) {
                // Up
                next_x = bomb_x; next_y = bomb_y - j;
                if (up == j && next_y >= 0 && game.map[next_y][next_x] == MAP_TILE_EMPTY) {
                    if (player_x == next_x && player_y == next_y)
                        return true;
                    up++;
                }

                // Down
                next_x = bomb_x; next_y = bomb_y + j;
                if (down == j && next_y < game.map_height && game.map[next_y][next_x] == MAP_TILE_EMPTY) {
                    if (player_x == next_x && player_y == next_y)
                        return true;
                    down++;
                }

                // Left
                next_x = bomb_x - j; next_y = bomb_y;
                if (left == j && next_x >= 0 && game.map[next_y][next_x] == MAP_TILE_EMPTY) {
                    if (player_x == next_x && player_y == next_y)
                        return true;
                    left++;
                }

                // Right
                next_x = bomb_x + j; next_y = bomb_y;
                if (right == j && next_x < game.map_width && game.map[next_y][next_x] == MAP_TILE_EMPTY) {
                    if (player_x == next_x && player_y == next_y)
                        return true;
                    right++;
                }
            }
        }
    }

    return false;
}



void check_game_end(void)
{
    if (game.player_count > 1)
        return;

    // Find the last player alive (the winner)
    int winner = 0;
    while (!game.players[winner].alive)
        winner++;

    printf("Player %d won!\n", winner + 1);

    game.ended = true;

    // Tell the clients to stop waiting for messages
    for (int i = 0; i < game.number_of_players; i++)
        send_game_state(get_client_msqid(game.players[i].pid_client), game, true);

    // Send the winner to the clients
    for (int i = 0; i < game.number_of_players; i++)
        send_game_end(get_client_msqid(game.players[i].pid_client), winner);
}



void retrieve_map_name(char* path_to_map)
{
    char* p = strrchr(path_to_map, '/'); // Find the last occurrence
    if (p == NULL)
        // '/' not found, so the file name is the entire string
        strcpy(game.map_name, path_to_map);
    else
        // The file name is the character after the last '/'
        strcpy(game.map_name, p + 1);
}
