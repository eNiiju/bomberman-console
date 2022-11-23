/**
 * Memory.c header file.
 * @file memory.h
 * @date 2022-11-23
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/shm.h>

/* ------------------------------------------------------------------------- */
/*                          Constants & definitions                          */
/* ------------------------------------------------------------------------- */

#define PROJECT_ID 1234
#define SHM_SIZE 1024

/* ------------------------------------------------------------------------- */
/*                            Function prototypes                            */
/* ------------------------------------------------------------------------- */

/**
 * Create the shared memory and put the server's PID on the first line.
 * @param server_pid The server's PID.
 * @return The shared memory's ID.
*/
int createSharedMemory(pid_t server_pid);
