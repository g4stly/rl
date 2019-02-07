#include <stdlib.h>
#include "tiles.h"
#include "../../util.h"


const char *const tile_names[TILES] = {"wall", "floor", "water", "grass"};

void init_tiles() {
	tiles[WALL_TILE] = malloc(sizeof(struct Tile));
	if (!tiles[WALL_TILE]) { die("initialize_tiles(): malloc():"); }
	tiles[WALL_TILE]->ch = '#';
	tiles[WALL_TILE]->wall = 1;
	tiles[WALL_TILE]->color = WHITEBLACK;

	tiles[FLOOR_TILE] = malloc(sizeof(struct Tile));
	if (!tiles[FLOOR_TILE]) { die("initialize_tiles(): malloc():"); }
	tiles[FLOOR_TILE]->ch = '.';
	tiles[FLOOR_TILE]->wall = 0;
	tiles[FLOOR_TILE]->color = WHITEBLACK;

	tiles[WATER_TILE] = malloc(sizeof(struct Tile));
	if (!tiles[WATER_TILE]) { die("initialize_tiles(): malloc():"); }
	tiles[WATER_TILE]->ch = '.';
	tiles[WATER_TILE]->wall = 1;
	tiles[WATER_TILE]->color = BLACKBLUE;

	tiles[GRASS_TILE] = malloc(sizeof(struct Tile));
	if (!tiles[GRASS_TILE]) { die("initialize_tiles(): malloc():"); }
	tiles[GRASS_TILE]->ch = '"';
	tiles[GRASS_TILE]->wall = 0;
	tiles[GRASS_TILE]->color = GREENBLACK;
}

void destroy_tiles() {
	free(tiles[WALL_TILE]);
	free(tiles[FLOOR_TILE]);
	free(tiles[WATER_TILE]);
	free(tiles[GRASS_TILE]);
}
