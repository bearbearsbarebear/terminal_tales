#include "game.h"
#include "render.h"
#include "lib/curses.h"

void render_stats(struct Game* game)
{
	char text[100];
	struct Player player = game->player;
	sprintf(text, "Health: %d\tAttack: %d\tDefense: %d\tLevel: %d", player.health, player.offense, player.defense, player.level);

	mvprintw(LINES - 1, 0, "%s", text);
}

void render_map(struct Game* game)
{
	int centered_x = game->player.pos.x;
	int centered_y = game->player.pos.y;
	int camera_radius = CAMERA_WIDTH / 2;

	int window_y = 0;
	int window_x = 0;
	for (int y = centered_y - camera_radius; y < centered_y + camera_radius; ++y) {
		if (y < 0 || y >= MAP_SIZE) continue;
		
		window_x = 0;

		for (int x = centered_x - camera_radius; x < centered_x + camera_radius; ++x) {
			if (x < 0 || x >= MAP_SIZE) continue;
			int map_index = y * MAP_SIZE + x;
			int map_value = game->map[map_index];

			// Render based on map value
			if (map_value == 0) { // 0 = empty
				attron(COLOR_PAIR(3));
				mvaddch(window_y, window_x, ' ');
				attroff(COLOR_PAIR(3));
			} else if (map_value == 1) { // 1 = player
				attron(COLOR_PAIR(2));
				mvaddch(window_y, window_x, ' ');
				attroff(COLOR_PAIR(2));
			} else if (map_value == 2) { // 2 = creature
				attron(COLOR_PAIR(4));
				mvaddch(window_y, window_x, '@');
				attroff(COLOR_PAIR(4));
			} else if (map_value == 5) { // 5 = wall
				attron(COLOR_PAIR(1));
				mvaddch(window_y, window_x, ' ');
				attroff(COLOR_PAIR(1));
			}

			++window_x;
		}

		++window_y;
	}
}