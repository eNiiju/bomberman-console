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

/**
 * Prints a tile on the ncurses window.
 * @param x The x coordinate of the tile
 * @param y The y coordinate of the tile
 * @param c The character to print
*/
void print_tile(int x, int y, char c);

/**
 * Prints informations about the game under the map.
 * @param game The game state
*/
void print_informations(struct game* game);
