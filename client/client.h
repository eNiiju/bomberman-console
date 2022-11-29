/**
 * Client header file.
 * @file client.h
 * @date 2022-11-16
*/

#include <stdio.h>
#include <pthread.h>

#include "../include/definitions.h"
#include "message.h"

/* ------------------------------------------------------------------------- */
/*                            Function prototypes                            */
/* ------------------------------------------------------------------------- */

void* thread_handle_messages(void* arg);
