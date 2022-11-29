/**
 * Message type structure definitions for the client and the server.
 * @file message_types.h
 * @date 2022-11-29
*/

#include <sys/types.h>

/* ------------------------------------------------------------------------- */
/*                        Messages sent by the client                        */
/* ------------------------------------------------------------------------- */

struct message_connection {
    long mtype; // MESSAGE_TYPE_CLIENT_CONNECTION
    struct {
        pid_t pid; // PID of the client
    } mcontent;
};

struct message_disconnection {
    long mtype; // MESSAGE_TYPE_CLIENT_DISCONNECTION
    struct {
        pid_t pid; // PID of the client
    } mcontent;
};

struct message_move {
    long mtype; // MESSAGE_TYPE_CLIENT_MOVE
    struct {
        pid_t pid; // PID of the client
        int direction; // DIRECTION_UP, DIRECTION_DOWN, DIRECTION_LEFT or DIRECTION_RIGHT
    } mcontent;
};

struct message_place_bomb {
    long mtype; // MESSAGE_TYPE_CLIENT_PLACE_BOMB
    struct {
        pid_t pid; // PID of the client
    } mcontent;
};

/* ------------------------------------------------------------------------- */
/*                        Messages sent by the server                        */
/* ------------------------------------------------------------------------- */

struct message_server {
    long mtype; // PID of the client
    struct {
        pid_t pid; // PID of the server
        int type; // MESSAGE_TYPE_SERVER_*
        bool success; // true if the request was successful, false otherwise
        struct game game; // Game state
    } mcontent;
};
