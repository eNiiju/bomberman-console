/**
 * Functions handling displaying things on the client's screen.
 * @file display.c
 * @date 2023-01-01
*/

#include "display.h"

/* ------------------------------------------------------------------------- */
/*                                 Functions                                 */
/* ------------------------------------------------------------------------- */

void display_map(struct game* game)
{
    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int n = 0; n < 2; n++) { // Print 2 times each line
            for (int j = 0; j < MAP_WIDTH; j++) {
                switch (game->map[i][j]) {
                case MAP_TILE_WALL:           printw("####"); break;
                case MAP_TILE_BREAKABLE_WALL: printw("////"); break;
                case MAP_TILE_EMPTY:          printw("    "); break;
                }
            }
            move(i * 2 + n + 1, 0);
        }
    }
}



void display_bombs(struct game* game)
{
    for (int i = 0; i < game->player_count; i++) {
        if (!game->players[i].alive || !game->players[i].bomb.active)
            continue;

        if (game->players[i].bomb.exploded) {
            // TODO: Display the explosion fire

        }
        else {
            // Display the bomb as planted
            int x = game->players[i].bomb.coords.x;
            int y = game->players[i].bomb.coords.y;
            move(y * 2, x * 4);
            printw(" oo ");
            move(y * 2 + 1, x * 4);
            printw(" oo ");
        }
    }
}



void display_players(struct game* game)
{
    for (int i = 0; i < game->player_count; i++) {
        if (!game->players[i].alive)
            continue;

        int x = game->players[i].coords.x;
        int y = game->players[i].coords.y;
        move(y * 2, x * 4);
        for (int n = 0; n < 4; n++)
            addch((char)(i + 1 + 48));
        move(y * 2 + 1, x * 4);
        for (int n = 0; n < 4; n++)
            addch((char)(i + 1 + 48));
    }

}
