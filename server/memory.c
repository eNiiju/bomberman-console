/**
 * Functions handling shared memory.
 * @file memory.c
 * @date 2022-11-23
*/

#include "memory.h"

/* ------------------------------------------------------------------------- */
/*                             Global variables                              */
/* ------------------------------------------------------------------------- */



/* ------------------------------------------------------------------------- */
/*                                 Functions                                 */
/* ------------------------------------------------------------------------- */

int createSharedMemory(pid_t server_pid)
{
    printf("Creating  shared memory...");

    key_t key = ftok("/etc/passwd", PROJECT_ID);
    size_t size = SHM_SIZE * sizeof(char);
    int shmflag = 0666 | IPC_CREAT | IPC_EXCL;

    int shmid = shmget(key, size, shmflag);
    char* shm = shmat(shmid, NULL, 0);
    sprintf(shm, "%s", itoa(server_pid));

    printf("OK\n");

    return shmid;
}
