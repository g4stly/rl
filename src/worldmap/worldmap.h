#ifndef worldmap_h_
#define worldmap_h_

#include "../player/player.h"
#include "../interface/interface.h"

#define CMD_NAME_LEN 	12

struct Tile {
	char ch;
	int wall;
};

struct Map {
	// array
	int cols, rows;
	struct Tile **map;
};

struct WorldMap {
	struct Map levels[5];	
	struct Player *player;
};

struct Command {
	char name[CMD_NAME_LEN];
	int (*func)(struct Interface *ui, struct WorldMap *m, char **argv);
};


void worldMap_Init(struct WorldMap *map);

int worldMap_GetInput(
	struct Interface *ui, 
	struct WorldMap *map,
	struct InterfaceInput *input);

void worldMap_Step(
	struct Interface *ui, 
	struct WorldMap *map, 
	struct InterfaceInput *input);

void worldMap_Draw(
	struct Interface *ui, 
	struct WorldMap *map);

void worldMap_Shutdown(struct WorldMap *map);

// load_level.c
void load_level(struct Map *m, const char *filename);
void unload_level(struct Map *m);

// commands.c
void command_try(struct Command *cmd,
	const char *word,
	char **words);

int command(struct Interface *ui,
	struct WorldMap *m,
	const char *word,
	char **words);
void load_commands(void);
void unload_commands(void);

#endif
