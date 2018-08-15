#ifndef worldmap_h_
#define worldmap_h_

#include "../player/player.h"
#include "../interface/interface.h"

#define CMD_NAME_LEN 12

struct WorldMap {
	int array[64];
	struct Player *player;
};

struct Command {
	char name[CMD_NAME_LEN];
	int (*func)(struct Interface *ui, struct WorldMap *m, char **argv);
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

void worldMap_Shutdown(void);

// these are defined in commands.c
void command_try(struct Command *cmd,
	const char *word,
	char **words);
int command(struct Interface *ui,
	const char *word,
	char **words);
void load_commands(void);
void unload_commands(void);

#endif
