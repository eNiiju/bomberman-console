/**
 * Functions handling displaying things on the client's screen.
 * @file display.c
 * @date 2023-01-01
*/

#include "display.h"

/* ------------------------------------------------------------------------- */
/*                                 Functions                                 */
/* ------------------------------------------------------------------------- */

void init_display()
{
    initscr(); // Initialize the ncurses window
    noecho(); // Disable echoing of typed characters
    start_color(); // Enable colors

    // Initialize the color pairs (foreground + background)
    init_pair(MAP_TILE_COLOR_EMPTY, COLOR_BLACK, COLOR_BLACK);
    init_pair(MAP_TILE_COLOR_WALL, COLOR_WHITE, COLOR_BLACK);
    init_pair(MAP_TILE_COLOR_BREAKABLE_WALL, COLOR_BLACK, COLOR_WHITE);
    init_pair(MAP_TILE_COLOR_BOMB, COLOR_YELLOW, COLOR_BLACK);
    init_pair(MAP_TILE_COLOR_EXPLOSION, COLOR_RED, COLOR_BLACK);
    init_pair(MAP_TILE_COLOR_POWERUP, COLOR_WHITE, COLOR_GREEN);
    init_pair(MAP_TILE_COLOR_PLAYER_1, COLOR_BLUE, COLOR_BLACK);
    init_pair(MAP_TILE_COLOR_PLAYER_2, COLOR_GREEN, COLOR_BLACK);
    init_pair(MAP_TILE_COLOR_PLAYER_3, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(MAP_TILE_COLOR_PLAYER_4, COLOR_CYAN, COLOR_BLACK);
    init_pair(TEXT_COLOR_TITLE, COLOR_BLACK, COLOR_WHITE);
    init_pair(TEXT_COLOR_MAP_NAME, COLOR_YELLOW, COLOR_BLACK);
    init_pair(TEXT_COLOR_CONTROL_KEY, COLOR_GREEN, COLOR_BLACK);
}



void display_map(struct game* game)
{
    for (int line = 0; line < game->map_height; line++)
        for (int column = 0; column < game->map_width; column++)
            switch (game->map[line][column]) {
            case MAP_TILE_EMPTY: print_tile(column, line, game->map[line][column], MAP_TILE_COLOR_EMPTY); break;
            case MAP_TILE_WALL: print_tile(column, line, game->map[line][column], MAP_TILE_COLOR_WALL); break;
            case MAP_TILE_BREAKABLE_WALL: print_tile(column, line, game->map[line][column], MAP_TILE_COLOR_BREAKABLE_WALL); break;
            }

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

            print_tile(x, y, MAP_TILE_EXPLOSION, MAP_TILE_COLOR_EXPLOSION);

            for (int j = 1; j <= game->players[i].bomb.range; j++) {
                // Up
                next_x = x; next_y = y - j;
                if (up == j && next_y >= 0 && game->map[next_y][next_x] == MAP_TILE_EMPTY) {
                    print_tile(next_x, next_y, MAP_TILE_EXPLOSION, MAP_TILE_COLOR_EXPLOSION);
                    up++;
                }

                // Down
                next_x = x; next_y = y + j;
                if (down == j && next_y < game->map_height && game->map[next_y][next_x] == MAP_TILE_EMPTY) {
                    print_tile(next_x, next_y, MAP_TILE_EXPLOSION, MAP_TILE_COLOR_EXPLOSION);
                    down++;
                }

                // Left
                next_x = x - j; next_y = y;
                if (left == j && next_x >= 0 && game->map[next_y][next_x] == MAP_TILE_EMPTY) {
                    print_tile(next_x, next_y, MAP_TILE_EXPLOSION, MAP_TILE_COLOR_EXPLOSION);
                    left++;
                }

                // Right
                next_x = x + j; next_y = y;
                if (right == j && next_x < game->map_width && game->map[next_y][next_x] == MAP_TILE_EMPTY) {
                    print_tile(next_x, next_y, MAP_TILE_EXPLOSION, MAP_TILE_COLOR_EXPLOSION);
                    right++;
                }
            }
        }
        else // Display the bomb as planted
            print_tile(x, y, MAP_TILE_BOMB, MAP_TILE_COLOR_BOMB);
    }
}



void display_players(struct game* game)
{
    for (int i = 0; i < game->number_of_players; i++) {
        if (!game->players[i].alive)
            continue;

        int x = game->players[i].coords.x;
        int y = game->players[i].coords.y;

        print_tile(x, y, MAP_TILE_PLAYER_1 + i, MAP_TILE_COLOR_PLAYER_1 + i);
    }
}



void print_tile(int x, int y, char c, int color_pair)
{
    attron(COLOR_PAIR(color_pair)); // Set the color pair
    for (int n = 0; n < MAP_TILE_SIZE_Y; n++) {
        move(y * MAP_TILE_SIZE_Y + n, x * MAP_TILE_SIZE_X);
        for (int n = 0; n < MAP_TILE_SIZE_X; n++)
            addch(c);
    }
    attroff(COLOR_PAIR(color_pair)); // Reset the color pair
}



void display_informations(struct game* game, int player_number)
{
    move(0, game->map_width * MAP_TILE_SIZE_X + 2);
    attron(COLOR_PAIR(TEXT_COLOR_TITLE));
    printw("BomberTTY");
    attroff(COLOR_PAIR(TEXT_COLOR_TITLE));

    move(1, game->map_width * MAP_TILE_SIZE_X + 2);
    printw("You're playing on the map ");
    attron(COLOR_PAIR(TEXT_COLOR_MAP_NAME));
    printw("%s\n", game->map_name);
    attroff(COLOR_PAIR(TEXT_COLOR_MAP_NAME));

    move(3, game->map_width * MAP_TILE_SIZE_X + 2);
    printw("Welcome to the game ! Your goal is to kill\n");

    move(4, game->map_width * MAP_TILE_SIZE_X + 2);
    printw("your opponents by placing bombs on the map.\n");

    move(5, game->map_width * MAP_TILE_SIZE_X + 2);
    printw("You can also break walls !\n");

    move(6, game->map_width * MAP_TILE_SIZE_X + 2);
    printw("Power-ups increase your bomb range.\n");

    move(8, game->map_width * MAP_TILE_SIZE_X + 2);
    attron(COLOR_PAIR(TEXT_COLOR_CONTROL_KEY));
    printw("%c", CONTROL_KEY_UP);
    attroff(COLOR_PAIR(TEXT_COLOR_CONTROL_KEY));
    printw(" : Move up\n");

    move(9, game->map_width * MAP_TILE_SIZE_X + 2);
    attron(COLOR_PAIR(TEXT_COLOR_CONTROL_KEY));
    printw("%c", CONTROL_KEY_DOWN);
    attroff(COLOR_PAIR(TEXT_COLOR_CONTROL_KEY));
    printw(" : Move down\n");

    move(10, game->map_width * MAP_TILE_SIZE_X + 2);
    attron(COLOR_PAIR(TEXT_COLOR_CONTROL_KEY));
    printw("%c", CONTROL_KEY_LEFT);
    attroff(COLOR_PAIR(TEXT_COLOR_CONTROL_KEY));
    printw(" : Move left\n");

    move(11, game->map_width * MAP_TILE_SIZE_X + 2);
    attron(COLOR_PAIR(TEXT_COLOR_CONTROL_KEY));
    printw("%c", CONTROL_KEY_RIGHT);
    attroff(COLOR_PAIR(TEXT_COLOR_CONTROL_KEY));
    printw(" : Move right\n");

    move(12, game->map_width * MAP_TILE_SIZE_X + 2);
    attron(COLOR_PAIR(TEXT_COLOR_CONTROL_KEY));
    printw("SPACE");
    attroff(COLOR_PAIR(TEXT_COLOR_CONTROL_KEY));
    printw(" : Place a bomb\n");

    move(13, game->map_width * MAP_TILE_SIZE_X + 2);
    attron(COLOR_PAIR(TEXT_COLOR_CONTROL_KEY));
    printw("%c", CONTROL_KEY_REFRESH);
    attroff(COLOR_PAIR(TEXT_COLOR_CONTROL_KEY));
    printw(" : Refresh the display\n");

    move(15, game->map_width * MAP_TILE_SIZE_X + 2);
    printw("%d/%d players alive\n", game->player_count, game->number_of_players);

    int x = game->players[player_number].coords.x;
    int y = game->players[player_number].coords.y;

    move(16, game->map_width * MAP_TILE_SIZE_X + 2);
    printw("You are ");
    attron(COLOR_PAIR(MAP_TILE_COLOR_PLAYER_1 + player_number));
    printw("player %d", player_number + 1);
    attroff(COLOR_PAIR(MAP_TILE_COLOR_PLAYER_1 + player_number));
    printw(" at (%d, %d)\n", x, y);

    move(17, game->map_width * MAP_TILE_SIZE_X + 2);
    printw("Your bomb range : ");
    attron(COLOR_PAIR(MAP_TILE_COLOR_BOMB));
    printw("%d\n", game->players[player_number].bomb_range);
    attroff(COLOR_PAIR(MAP_TILE_COLOR_BOMB));

    if (!game->players[player_number].alive) {
        move(19, game->map_width * MAP_TILE_SIZE_X + 2);
        attron(COLOR_PAIR(MAP_TILE_COLOR_EXPLOSION));
        printw("You are dead !\n");
        attroff(COLOR_PAIR(MAP_TILE_COLOR_EXPLOSION));
    }
}



void display_powerups(struct game* game)
{
    for (int i = 0; i < game->powerup_count; i++) {
        if (!game->powerups[i].active)
            continue;

        int x = game->powerups[i].coords.x;
        int y = game->powerups[i].coords.y;

        print_tile(x, y, MAP_TILE_POWERUP, MAP_TILE_COLOR_POWERUP);
    }
}
