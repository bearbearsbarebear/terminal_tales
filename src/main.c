#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "render.h"
#include "lib/curses.h"

int main(int argc, char* argv[])
{
	// Seeding RNG
	srand(time(NULL));

	// Initializing curses screen
	initscr();

	start_color();  				// Enable color functionality
	init_pair(1, COLOR_BLACK, COLOR_YELLOW);   	// Dark orange
	init_pair(2, COLOR_BLACK, COLOR_CYAN);     	// Light blue
	init_pair(3, COLOR_WHITE, COLOR_BLACK);    	// Black
	init_pair(4, COLOR_BLACK, COLOR_RED);    	// Red

	// Disable character echoing
	cbreak();
	noecho();

	// Enable keypad input for special keys
	keypad(stdscr, TRUE);

	// Game initialization
	struct Game* game = game_init();
	if (game == NULL) {
		printf("Exiting on initialization error.\n");
		return 1;
	}

	// Main game loop
	while (game->running) {
		switch (getch()) {
			case KEY_UP:
				player_move(game, 1);
				break;
			case KEY_RIGHT:
				player_move(game, 2);
				break;
			case KEY_DOWN:
				player_move(game, 3);
				break;
			case KEY_LEFT:
				player_move(game, 4);
				break;
			case ' ':
				game->running = false;
				break;
			default:
				break;
		}

		clear();
		render_map(game);
		render_stats(game);
		refresh();
	}

	// Cleanup and exit
	endwin();

	return 0;
}
