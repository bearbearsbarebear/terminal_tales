#include "game.h"

bool is_cell_valid(struct Game* game, int x, int y)
{
	return x >= 0 && x < MAP_SIZE && y >= 0 && y < MAP_SIZE && game->map[y * MAP_SIZE + x] != 1;
}

// Function to generate map using randomized DFS
// Very simplistic and it's basically a "corridor generator"
void generate_map(struct Game* game)
{
	// Set the whole map with walls initially
	for (int y = 0; y < MAP_SIZE; ++y) {
		for (int x = 0; x < MAP_SIZE; ++x) {
			game->map[y * MAP_SIZE + x] = 5;
		}
	}

	// Set the first position for the DFS to start
	int starting_x = 1;
	int starting_y = 1;
	game->map[starting_y * MAP_SIZE + starting_x] = 0;

	int dx[] = {-2, 2, 0, 0};
	int dy[] = {0, 0, -2, 2};

	// Stack and visited arrays for the DFS
	int stack_size = (MAP_SIZE * MAP_SIZE) / 4;
	int* stack = (int*) malloc(stack_size * sizeof(int));
	bool* visited = (bool*) malloc(MAP_SIZE * MAP_SIZE * sizeof(bool));

	for (int i = 0; i < MAP_SIZE * MAP_SIZE; ++i) {
		visited[i] = false;
	}

	int top = 0;
	stack[top] = starting_y * MAP_SIZE + starting_x;
	visited[starting_y * MAP_SIZE + starting_x] = true;

	// DFS algorithm
	while (top >= 0) {
		int currentX = stack[top] % MAP_SIZE;       // X-coordinate of current position
		int currentY = stack[top] / MAP_SIZE;       // Y-coordinate of current position
		top--;

		// Shuffle the directions randomly
		for (int i = 0; i < 4; ++i) {
			int randIndex = (rand() % 4 + i) % 4;
			int temp = dx[i];
			dx[i] = dx[randIndex];
			dx[randIndex] = temp;

			temp = dy[i];
			dy[i] = dy[randIndex];
			dy[randIndex] = temp;
		}

		// Check neighbors in shuffled order
		for (int i = 0; i < 4; ++i) {
			int nextX = currentX + dx[i];           // X-coordinate of neighboring position
			int nextY = currentY + dy[i];           // Y-coordinate of neighboring position

			// Check if neighboring position is within the bounds of the map
			if (is_cell_valid(game, nextX, nextY)) {
				// Check if neighboring position has not been visited
				if (!visited[nextY * MAP_SIZE + nextX]) {
                    // Calculate the middle position
                    int middleX = currentX + dx[i] / 2;
                    int middleY = currentY + dy[i] / 2;

                    // Set the positions as corridor
                    game->map[nextY * MAP_SIZE + nextX] = 0;                // Set neighboring position as corridor
                    game->map[middleY * MAP_SIZE + middleX] = 0; 

                    // Push neighboring position onto the stack
                    top++;
                    stack[top] = nextY * MAP_SIZE + nextX;

                    // Mark neighboring position and middle position as visited
                    visited[nextY * MAP_SIZE + nextX] = true;
                    visited[middleY * MAP_SIZE + middleX] = true;

                    /*
					game->map[nextY * MAP_SIZE + nextX] = 0;                                    // Set neighboring position as corridor
					game->map[(currentY + dy[i] / 2) * MAP_SIZE + (currentX + dx[i] / 2)] = 0;  // Set position in between as corridor
					top++;
					stack[top] = nextY * MAP_SIZE + nextX;                                      // Push neighboring position onto the stack
					visited[nextY * MAP_SIZE + nextX] = true;                                   // Mark neighboring position as visited
					*/
				}
			}
		}
	}


	free(stack);
	free(visited);

	// Set player's coordinate as value 1
	game->map[game->player.pos.y * MAP_SIZE + game->player.pos.x] = 1;
}

// With the way is_cell_valid works, it will end up spawning mobs inside of walls
// Subsequently deleting the walls from there
// This is not bad, considering with that we can open up new paths in case the player
// kills the mobs
void spawn_creatures(struct Game* game)
{
	for (int i = 0; i < game->num_creatures; ++i) {
		game->creatures[i].health = 10;
		game->creatures[i].offense = 2;
		game->creatures[i].defense = 2;
		game->creatures[i].pos.x = rand() % MAP_SIZE;
		game->creatures[i].pos.y = rand() % MAP_SIZE;
		while (!is_cell_valid(game, game->creatures[i].pos.x, game->creatures[i].pos.y)) {
			game->creatures[i].pos.x = rand() % MAP_SIZE;
			game->creatures[i].pos.y = rand() % MAP_SIZE;
		}
		game->map[game->creatures[i].pos.y * MAP_SIZE + game->creatures[i].pos.x] = 2;  // Set creature position in map
	}
}

struct Creature find_creature(struct Game* game, int x, int y)
{
	struct Creature creature;
	for (int i = 0; i < game->num_creatures; ++i) {
		if (game->creatures[i].pos.x == x && game->creatures[i].pos.y == y) {
			creature = game->creatures[i];
			break;
		}
	}

	return creature;
}

void remove_creature(struct Game* game, int x, int y)
{
	for (int i = 0; i < game->num_creatures; ++i) {
		if (game->creatures[i].pos.x == x && game->creatures[i].pos.y == y) {
			// Shift the remaining creatures in the array to fill the gap
			for (int j = i; j < game->num_creatures - 1; ++j) {
				game->creatures[j] = game->creatures[j + 1];
			}

			// Decrease the number of creatures
			game->num_creatures--;
			break;
		}
	}
}

struct Game* game_init()
{
	struct Game* game = (struct Game*) malloc(sizeof(struct Game));;
	if (game == NULL) {
		printf("Error: Memory allocation failed.\n");
		return NULL;
	}

	// Initialize player
	game->player.health = 100;
	game->player.offense = 1;
	game->player.defense = 1;
	game->player.level = 1;
	game->player.pos.x = 1;
	game->player.pos.y = 1;

	// Initialize map
	game->map = (int*) malloc(MAP_SIZE * MAP_SIZE * sizeof(int));
	if (game->map == NULL) {
		printf("Error: Memory allocation failed.\n");
		free(game);
		return NULL;
	}

	generate_map(game);

	// Initialize creatures
	game->num_creatures = CREATURES_QUANTITY;
	game->creatures = (struct Creature*) malloc(game->num_creatures * sizeof(struct Creature));
	if (game->creatures == NULL) {
		printf("Error: Memory allocation failed.\n");
		free(game);
		return NULL;
	}

	spawn_creatures(game);

	// Start position of the camera
	game->camera_x = 11;
	game->camera_y = 11;

	// Rather the game is running or not
	game->running = true;

	return game;
}

// direction
// 1 = north
// 2 = east
// 3 = south
// 4 = west
void player_move(struct Game* game, int direction)
{
	struct Position old_pos = game->player.pos;
	struct Position new_pos = game->player.pos;

	switch (direction) {
		case 1:
			new_pos.y = new_pos.y - 1;
			break;
		case 2:
			new_pos.x = new_pos.x + 1;
			break;
		case 3:
			new_pos.y = new_pos.y + 1;
			break;
		case 4:
			new_pos.x = new_pos.x - 1;
			break;
	}

	if (new_pos.x < 0 || new_pos.x >= MAP_SIZE) return;
	if (new_pos.y < 0 || new_pos.y >= MAP_SIZE) return;
	if (game->map[new_pos.y * MAP_SIZE + new_pos.x] == 5) return; // hits a wall

	// Move onto enemy
	if (game->map[new_pos.y * MAP_SIZE + new_pos.x] == 2) {
		struct Creature creature = find_creature(game, new_pos.x, new_pos.y);
		int player_damage_taken = creature.offense - game->player.defense;
		if (game->player.health - player_damage_taken <= 0) {
			game->running = false;
			return;
		}

		remove_creature(game, new_pos.x, new_pos.y);
		game->player.health -= player_damage_taken;
	}

	game->player.pos = new_pos;
	game->map[new_pos.y * MAP_SIZE + new_pos.x] = 1;
	game->map[old_pos.y * MAP_SIZE + old_pos.x] = 0;
}
