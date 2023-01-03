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
    for (int line = 0; line < game->map_height; line++)
        for (int column = 0; column < game->map_width; column++)
            print_tile(column, line, game->map[line][column]);
}



void display_bombs(struct game* game)
{
    for (int i = 0; i < game->number_of_players; i++) {
        if (!game->players[i].alive || !game->players[i].bomb.active)
            continue;

        int x = game->players[i].bomb.coords.x;
        int y = game->players[i].bomb.coords.y;

        if (game->players[i].bomb.exploded) {
            // Display the explosion fire
            int next_x, next_y;
            int up = 1, down = 1, left = 1, right = 1;

            print_tile(x, y, MAP_TILE_EXPLOSION);

            for (int j = 1; j <= game->players[i].bomb.range; j++) {
                // Up
                next_x = x; next_y = y - j;
                if (up == j && next_y >= 0 && game->map[next_y][next_x] == MAP_TILE_EMPTY) {
                    print_tile(next_x, next_y, MAP_TILE_EXPLOSION);
                    up++;
                }

                // Down
                next_x = x; next_y = y + j;
                if (down == j && next_y < game->map_height && game->map[next_y][next_x] == MAP_TILE_EMPTY) {
                    print_tile(next_x, next_y, MAP_TILE_EXPLOSION);
                    down++;
                }

                // Left
                next_x = x - j; next_y = y;
                if (left == j && next_x >= 0 && game->map[next_y][next_x] == MAP_TILE_EMPTY) {
                    print_tile(next_x, next_y, MAP_TILE_EXPLOSION);
                    left++;
                }

                // Right
                next_x = x + j; next_y = y;
                if (right == j && next_x < game->map_width && game->map[next_y][next_x] == MAP_TILE_EMPTY) {
                    print_tile(next_x, next_y, MAP_TILE_EXPLOSION);
                    right++;
                }
            }
        }
        else // Display the bomb as planted
            print_tile(x, y, MAP_TILE_BOMB);
    }
}



void display_players(struct game* game)
{
    for (int i = 0; i < game->number_of_players; i++) {
        if (!game->players[i].alive)
            continue;

        int x = game->players[i].coords.x;
        int y = game->players[i].coords.y;
        print_tile(x, y, (char)(i + 1 + 48)); // Print the player number
    }
}



void print_tile(int x, int y, char c)
{
    for (int n = 0; n < MAP_TILE_SIZE_Y; n++) {
        move(y * MAP_TILE_SIZE_Y + n, x * MAP_TILE_SIZE_X);
        for (int n = 0; n < MAP_TILE_SIZE_X; n++)
            addch(c);
    }
}



void display_informations(struct game* game, int player_number)
{
    move(0, game->map_width * MAP_TILE_SIZE_X + 2);
    printw("BomberTTY");

    move(1, game->map_width * MAP_TILE_SIZE_X + 2);
    printw("You're playing on the map %s\n", game->map_name);

    move(3, game->map_width * MAP_TILE_SIZE_X + 2);
    printw("Welcome to the game ! your goal is to kill\n");

    move(4, game->map_width * MAP_TILE_SIZE_X + 2);
    printw("your opponents by placing bombs on the map.\n");

    move(6, game->map_width * MAP_TILE_SIZE_X + 2);
    printw("- Move up : %c\n", CONTROL_KEY_UP);

    move(7, game->map_width * MAP_TILE_SIZE_X + 2);
    printw("- Move down : %c\n", CONTROL_KEY_DOWN);

    move(8, game->map_width * MAP_TILE_SIZE_X + 2);
    printw("- Move left : %c\n", CONTROL_KEY_LEFT);

    move(9, game->map_width * MAP_TILE_SIZE_X + 2);
    printw("- Move right : %c\n", CONTROL_KEY_RIGHT);

    move(10, game->map_width * MAP_TILE_SIZE_X + 2);
    printw("- Place a bomb : SPACE\n");

    move(11, game->map_width * MAP_TILE_SIZE_X + 2);
    printw("- Refresh the screen : %c\n", CONTROL_KEY_REFRESH);

    move(13, game->map_width * MAP_TILE_SIZE_X + 2);
    printw("Players alive : %d / %d\n", game->player_count, game->number_of_players);

    move(14, game->map_width * MAP_TILE_SIZE_X + 2);
    printw("You are : Player %d\n", player_number + 1);
}
