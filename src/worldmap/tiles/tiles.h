#ifndef tiles_h_ 
#define tiles_h_

#ifndef worldmap_h_
#include "../worldmap.h"
#endif

#define TILES 4

#define WALL_TILE 0
#define FLOOR_TILE 1
#define WATER_TILE 2
#define GRASS_TILE 3
extern const char *const tile_names[TILES];

void init_tiles();
void destroy_tiles();
struct Tile *tiles[TILES];

#endif
