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
pthread_mutex_t mutex_processing_signal = PTHREAD_MUTEX_INITIALIZER;
struct player players[MAX_PLAYERS];
long nb_players = 0;

/* ------------------------------------------------------------------------- */
/*                                 Functions                                 */
/* ------------------------------------------------------------------------- */

int main(int argc, char* argv[])
{
    pthread_t thgame;

    if (!setup()) {
        printf("Server setup failed.\n");
        return EXIT_FAILURE;
    }

    // Wait for clients to connect
    printf("Server is listening with PID %d\n", getpid());

    // Wait for the game to start
    pthread_mutex_lock(&mutex_start_game);

    // Start the game
    printf("Game started!\n");
    pthread_create(&thgame, NULL, thread_game, NULL);
    pthread_join(thgame, NULL);

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
    pthread_mutex_lock(&mutex_processing_signal);

    switch (signal_number) {
    case SIGUSR1:
        printf("Received SIGUSR1 by PID %d\n", info->si_pid);

        // If there's a slot left, take it
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

    pthread_mutex_unlock(&mutex_processing_signal);
}



void* thread_player(void* arg)
{
    long player_number = (long)arg;
    printf("Player %ld joined the game.\n", player_number);
}



void* thread_game(void* arg)
{

}



void create_player(pid_t client_pid)
{
    pthread_t thplayer;

    // Initialize the player
    players[nb_players].client_pid = client_pid;
    players[nb_players].coords.x = 0;
    players[nb_players].coords.y = 0;
    players[nb_players].alive = true;
    players[nb_players].bomb_amount = 1;
    players[nb_players].bomb_range = DEFAULT_BOMB_RANGE;

    // Create the player thread
    pthread_create(&thplayer, NULL, thread_player, (void*)nb_players);

    nb_players++;
}

