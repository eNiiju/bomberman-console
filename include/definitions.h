/**
 * Constants & definitions for the client and the server.
 * @file definitions.h
 * @date 2022-11-29
 */

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <stdbool.h>

/* ------------------------------------------------------------------------- */
/*                                 Constants                                 */
/* ------------------------------------------------------------------------- */

// Paths
#define TOKEN_PATH_NAME "/etc/passwd"
#define PATH_MAPS    "./assets/maps/"
#define MAX_LENGTH_PATH_TO_MAP    100

// Message types
#define MESSAGE_CLIENT_CONNECTION_TYPE    1
#define MESSAGE_CLIENT_DISCONNECTION_TYPE 2
#define MESSAGE_CLIENT_MOVE_TYPE          3
#define MESSAGE_CLIENT_PLACE_BOMB_TYPE    4
#define MESSAGE_SERVER_RESPONSE_TYPE      5
#define MESSAGE_SERVER_GAME_STATE_TYPE    6
#define MESSAGE_SERVER_GAME_END_TYPE      7

// Direction values
#define DIRECTION_UP    1
#define DIRECTION_DOWN  2
#define DIRECTION_LEFT  3
#define DIRECTION_RIGHT 4

// Game default values & constants
#define MAX_PLAYERS                2 // TODO: Change to 4
#define DEFAULT_BOMB_RANGE         3
#define MAP_WIDTH                 15
#define MAP_HEIGHT                15
#define BOMB_EXPLOSION_TIMER_MS 3000
#define PLAYER_MOVE_COOLDOWN_MS  100
#define BOMB_FIRE_TIME_MS       2000
#define REFRESH_RATE_MS          100

// Map tiles
#define MAP_TILE_EMPTY          ' '
#define MAP_TILE_WALL           '#'
#define MAP_TILE_BREAKABLE_WALL '/'
#define MAP_TILE_BOMB           'o'
#define MAP_TILE_EXPLOSION      'x'

// Player control keys
#define CONTROL_KEY_UP         'z'
#define CONTROL_KEY_DOWN       's'
#define CONTROL_KEY_LEFT       'q'
#define CONTROL_KEY_RIGHT      'd'
#define CONTROL_KEY_PLACE_BOMB ' '

/* ------------------------------------------------------------------------- */
/*                                Structures                                 */
/* ------------------------------------------------------------------------- */

struct coordinates {
    int x;
    int y;
};

struct bomb {
    bool active;
    struct coordinates coords;
    int range;
    bool exploded;
};

/**
 * Structure representing every informations about a player
 * during a game.
*/
struct player {
    pid_t pid_client;
    bool alive;
    struct coordinates coords;
    int bomb_range;
    struct bomb bomb;
};

/**
 * Structure responsible for holding all informations about
 * the game's current state.
*/
struct game {
    int game_code;
    int msqid; // Main message queue ID
    bool ended;
    int winner; // Winner's player number, -1 if no winner
    int player_count;
    char map[MAP_WIDTH][MAP_HEIGHT];
    struct player players[MAX_PLAYERS];
};

/**
 * Structure representing a move a player wants to make.
*/
struct player_move {
    int player_number;
    int direction; // DIRECTION_UP, DIRECTION_DOWN, DIRECTION_LEFT or DIRECTION_RIGHT
};

#endif
