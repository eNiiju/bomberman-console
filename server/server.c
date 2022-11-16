/**
 * Main server file.
 * @file server.c
 * @date 2022-11-16
*/

#include "server.h"

/* ------------------------------------------------------------------------- */
/*                             Global variables                              */
/* ------------------------------------------------------------------------- */

sem_t sem_slots_left; // Semaphore used to know if there are player slots left
pthread_mutex_t mutex_start_game = PTHREAD_MUTEX_INITIALIZER; // Unlocking this mutex will start the game

/* ------------------------------------------------------------------------- */
/*                                 Functions                                 */
/* ------------------------------------------------------------------------- */

int main(int argc, char *argv[])
{
    if (!init()) {
        printf("Server initialization failed.\n");
        return EXIT_FAILURE;
    }

    // Signal hangler
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigaction(SIGUSR1, &sa, NULL);
    
    // Wait for clients to connect
    printf("Server is listening with PID %d\n", getpid());
    
    // Wait for the game to start
    pthread_mutex_lock(&mutex_start_game);

    return 0;
}



bool init(void)
{
    sem_init(&sem_slots_left, 0, MAX_PLAYERS);
    pthread_mutex_lock(&mutex_start_game); // Prevent the game from starting until ordered to do so

    return true;
}



void signal_handler(int signal_number)
{
    switch(signal_number)
    {
        case SIGUSR1:
            printf("Received SIGUSR1\n");
            break;
    }
}
