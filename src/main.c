#include "player/player.h"
#include "worldmap/worldmap.h"
#include "interface/interface.h"
#include "interface/main_ui.h"

int main(int argc, char **argv)
{
	struct Player p;
	struct WorldMap m;
	m.player = &p;
	m.player->ch = '@';
	m.player->xpos = 1;
	m.player->ypos = 1;
	struct Interface i;
	main_ui = &i;

	interface_Init(&i);
	player_Init(&i, &p, argc, argv);
	worldMap_Init(&m);

	// fair warning; i was drunk when i wrote most of this
	int result;
	struct InterfaceInput input;

	worldMap_Draw(&i, &m);
	while ((result = worldMap_GetInput(&i, &m, &input))) {
		worldMap_Step(&i, &m, &input);
		worldMap_Draw(&i, &m);
	}

	worldMap_Shutdown(&m);
	interface_Shutdown(&i);
	fprintf(stderr, "thanks for playing!\n");

	return 0;
}
