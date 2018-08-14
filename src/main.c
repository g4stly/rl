#include "player/player.h"
#include "worldmap/worldmap.h"
#include "interface/interface.h"

int main(int argc, char **argv)
{
	struct Player p;
	struct WorldMap m;
	struct Interface i;

	interface_Init(&i);
	player_Init(&i, &p, argc, argv);
	worldMap_Init(&m, &p);

	// fair warning; i was drunk when i wrote most of this
	struct InterfaceInput input;
	while (worldMap_GetInput(&i, &m, &input)) {
		worldMap_Step(&i, &m, &input);
	}
	interface_Shutdown(&i);

	return 0;
}
