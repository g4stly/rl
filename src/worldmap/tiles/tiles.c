#include <stdlib.h>
#include "tiles.h"
#include "../../util.h"


const char *const tile_names[TILES] = {"wall", "floor"};

void init_tiles() {
	tiles[WALL_TILE] = malloc(sizeof(struct Tile));
	if (!tiles[WALL_TILE]) { die("initialize_tiles(): malloc():"); }
	tiles[WALL_TILE]->ch = '#';
	tiles[WALL_TILE]->wall = 1;

	tiles[FLOOR_TILE] = malloc(sizeof(struct Tile));
	if (!tiles[FLOOR_TILE]) { die("initialize_tiles(): malloc():"); }
	tiles[FLOOR_TILE]->ch = '.';
	tiles[FLOOR_TILE]->wall = 0;
}

void destroy_tiles() {
	free(tiles[WALL_TILE]);
	free(tiles[FLOOR_TILE]);
}
