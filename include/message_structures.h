/**
 * Message type structure definitions for the client and the server.
 * @file message_types.h
 * @date 2022-11-29
*/

#include <sys/types.h>

/* ------------------------------------------------------------------------- */
/*                        Messages sent by the client                        */
/* ------------------------------------------------------------------------- */

struct message_client_connection {
    long mtype; // MESSAGE_CLIENT_CONNECTION_TYPE
    struct {
        pid_t pid_client;
    } mcontent;
};

struct message_client_disconnection {
    long mtype; // MESSAGE_CLIENT_DISCONNECTION_TYPE
};

struct message_client_move {
    long mtype; // MESSAGE_CLIENT_MOVE_TYPE
    struct {
        int direction; // DIRECTION_UP, DIRECTION_DOWN, DIRECTION_LEFT or DIRECTION_RIGHT
    } mcontent;
};

struct message_client_place_bomb {
    long mtype; // MESSAGE_CLIENT_PLACE_BOMB_TYPE
};

/* ------------------------------------------------------------------------- */
/*                        Messages sent by the server                        */
/* ------------------------------------------------------------------------- */

struct message_server_response {
    long mtype; // PID of the client
    struct {
        bool success; // true if the request was successful, false otherwise
    } mcontent;
};

struct message_server_game_state {
    long mtype; // MESSAGE_SERVER_GAME_STATE_TYPE
    struct {
        bool stop; // true if the game is over, false otherwise
        struct game game_state;
    } mcontent;
};

struct message_server_game_end {
    long mtype; // MESSAGE_SERVER_GAME_END_TYPE
    struct {
        int winner; // Winner's player index
    } mcontent;
};
