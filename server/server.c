/**
 * Main server file.
*/

#include "server.h"

int main(int argc, char *argv[])
{
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigaction(SIGUSR1, &sa, NULL);
    

    // Wait for players to connect



    printf("Server is listening with PID %d\n", getpid());

    while(1);

    return 0;
}

void signal_handler(int signal_number)
{
    printf("Signal %d received\n", signal_number);
}
