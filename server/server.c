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
struct player players[MAX_PLAYERS];
unsigned int nb_players = 0;

/* ------------------------------------------------------------------------- */
/*                                 Functions                                 */
/* ------------------------------------------------------------------------- */

int main(int argc, char* argv[])
{
    if (!setup()) {
        printf("Server setup failed.\n");
        return EXIT_FAILURE;
    }

    // Wait for clients to connect
    printf("Server is listening with PID %d\n", getpid());

    // Wait for the game to start
    pthread_mutex_lock(&mutex_start_game);

    return 0;
}



bool setup(void)
{
    sem_init(&sem_slots_left, 0, MAX_PLAYERS);
    pthread_mutex_lock(&mutex_start_game); // Prevent the game from starting until ordered to do so

    // Signal hangler
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = signal_handler;
    sigaction(SIGUSR1, &sa, NULL);

    return true;
}



void signal_handler(int signal_number, siginfo_t* info, void* ucontext)
{
    switch (signal_number) {
    case SIGUSR1:
        printf("Received SIGUSR1 by PID %d\n", info->si_pid);

        // Check if there are slots left
        if (sem_trywait(&sem_slots_left) == 0) {
            printf("Creating player %d\n", info->si_pid);
            kill(info->si_pid, SIGUSR1); // Reply to the client (OK)
            create_player(info->si_pid);
        }
        else {
            printf("No slots left for player %d\n", info->si_pid);
            kill(info->si_pid, SIGUSR2); // Reply to the client (NOK)
        }
        break;
    }
}



void* thread_player(void* arg)
{

}



void* thread_game(void* arg)
{

}



void create_player(pid_t client_pid)
{
    unsigned int slot = nb_players;
    nb_players++;

    // Initialize the player
    players[slot].client_pid = client_pid;
    players[slot].coords.x = 0;
    players[slot].coords.y = 0;
    players[slot].alive = true;
    players[slot].bomb_amount = 1;
    players[slot].bomb_range = DEFAULT_BOMB_RANGE;
}

