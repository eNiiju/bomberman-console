/**
 * display.c header file.
 * @file display.h
 * @date 2023-01-01
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

#include "../include/definitions.h"

/* ------------------------------------------------------------------------- */
/*                            Function prototypes                            */
/* ------------------------------------------------------------------------- */

/**
 * Displays the map on the ncurses window.
 * @param game The game state
*/
void display_map(struct game* game);

/**
 * Displays the bombs on the ncurses window.
 * @param game The game state
*/
void display_bombs(struct game* game);

/**
 * Displays the players on the ncurses window.
 * @param game The game state
*/
void display_players(struct game* game);
