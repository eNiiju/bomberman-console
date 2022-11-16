/**
 * Server header file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <signal.h>

/* ------------------------------------------------------------------------- */
/*                          Constants & definitions                          */
/* ------------------------------------------------------------------------- */

#define MAX_PLAYERS 4
#define MIN_PLAYERS 2
#define DEFAULT_BOMB_RANGE 3
#define MAP_WIDTH 15
#define MAP_HEIGHT 15

/* ------------------------------------------------------------------------- */
/*                            Function prototypes                            */
/* ------------------------------------------------------------------------- */

void signal_handler(int signal_number);

void* thread_player(void* arg);

void* thread_game(void* arg);
