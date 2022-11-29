/**
 * Constants & definitions for the client and the server.
 * @file definitions.h
 * @date 2022-11-29
 */

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <stdbool.h>

#define TOKEN_PATH_NAME "/etc/passwd"

#define MESSAGE_TYPE_CLIENT_CONNECTION 1
#define MESSAGE_TYPE_CLIENT_DISCONNECTION 2
#define MESSAGE_TYPE_CLIENT_MOVE 3
#define MESSAGE_TYPE_CLIENT_PLACE_BOMB 4

#define MESSAGE_TYPE_SERVER_GAME_STATE 1
#define MESSAGE_TYPE_SERVER_GAME_END 2
#define MESSAGE_TYPE_SERVER_RESPONSE 3

#define DIRECTION_UP 1
#define DIRECTION_DOWN 2
#define DIRECTION_LEFT 3
#define DIRECTION_RIGHT 4

#define MAX_PLAYERS 4
#define DEFAULT_BOMB_RANGE 3
#define BOMB_TIMER 3
#define MAP_WIDTH 15
#define MAP_HEIGHT 15

struct coordinates {
    unsigned int x;
    unsigned int y;
};

struct player {
    pid_t client_pid;
    bool alive;
    struct coordinates coords;
    unsigned int bomb_amount;
    unsigned int bomb_range;
};

struct game {
    int game_code;
    int msqid;
    bool ended;
    int winner;
    struct player players[MAX_PLAYERS];
    unsigned int player_count;
    unsigned char map[MAP_WIDTH][MAP_HEIGHT];
};

#endif
