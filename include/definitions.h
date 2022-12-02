/**
 * Constants & definitions for the client and the server.
 * @file definitions.h
 * @date 2022-11-29
 */

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <stdbool.h>

#define TOKEN_PATH_NAME "/etc/passwd"

#define MESSAGE_CLIENT_CONNECTION_TYPE 1
#define MESSAGE_CLIENT_DISCONNECTION_TYPE 2
#define MESSAGE_CLIENT_MOVE_TYPE 3
#define MESSAGE_CLIENT_PLACE_BOMB_TYPE 4

#define MESSAGE_SERVER_RESPONSE_TYPE 5
#define MESSAGE_SERVER_GAME_STATE_TYPE 6
#define MESSAGE_SERVER_GAME_END_TYPE 7

#define DIRECTION_UP 1
#define DIRECTION_DOWN 2
#define DIRECTION_LEFT 3
#define DIRECTION_RIGHT 4

#define MAX_PLAYERS 4
#define DEFAULT_BOMB_RANGE 3
#define BOMB_TIMER 3
#define MAP_WIDTH 15
#define MAP_HEIGHT 15

struct player {
    pid_t pid_client;
    bool alive;
    struct coordinates {
        unsigned int x;
        unsigned int y;
    } coords;
    unsigned int bomb_amount;
    unsigned int bomb_range;
};

struct game {
    int game_code;
    int msqid; // Main message queue ID
    bool ended;
    int winner; // Winner's player index,-1 if no winner
    struct player players[MAX_PLAYERS];
    int player_count;
    char map[MAP_WIDTH][MAP_HEIGHT];
};

#endif
