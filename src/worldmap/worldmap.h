#ifndef worldmap_h_
#define worldmap_h_

#include "../player/player.h"
#include "../interface/interface.h"

struct WorldMap {
	int array[64];
	struct Player *player;
};

void worldMap_Init(
	struct WorldMap *map, 
	struct Player *player);

int worldMap_GetInput(
	struct Interface *ui, 
	struct WorldMap *map,
	struct InterfaceInput *input);

void worldMap_Step(
	struct Interface *ui, 
	struct WorldMap *map, 
	struct InterfaceInput *input);

#endif
