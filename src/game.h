#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAP_SIZE 100
#define CAMERA_WIDTH 20
#define CREATURES_QUANTITY 100

struct Position {
	int x;
	int y;
};

struct Player {
	int health;
	int offense;
	int defense;
	int level;
	struct Position pos;
};

struct Creature {
	int health;
	int offense;
	int defense;
	struct Position pos;
};

struct Game {
	struct Player player;
	struct Creature* creatures;
	int num_creatures;
	int* map;
	int camera_x;
	int camera_y;
	bool running;
};

struct Game* game_init();
struct Creature find_creature(struct Game* game, int x, int y);
void remove_creature(struct Game* game, int x, int y);
void render_stats(struct Game* game);
void render_map(struct Game* game);
void player_move(struct Game* game, int direction);

#endif
