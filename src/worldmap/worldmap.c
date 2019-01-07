#include <math.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "worldmap.h"
#include "tiles.h"
#include "../interface/interface.h"
#include "../util.h"


static int console_read(struct Interface *ui, struct WorldMap *m, struct InterfaceInput *input)
{
	ui->ReadLine(ui, input);
	ui->WriteLine(ui, input->content);
	int result;
	char **words = split_string(input->content, ' ');
	char *word = *words;

	// test first word against entire list of commands
	// they entered the one with the most collisions
	struct Command cmd;
	memset(&cmd, 0, sizeof(struct Command));
	command_try(&cmd, word, words);
	if (!cmd.func) {
		ui->WriteLine(ui, "%s: no such command found.", word);
		free_ptrArray(words);
		return 1;
	}

	result = cmd.func(ui, m, words);
	free_ptrArray(words);
	return result;
}


void worldMap_Init(struct WorldMap *map)
{
	init_tiles();
	load_commands();
	memset(map->levels, 0, sizeof(struct Map) * 5);
	load_level(&map->levels[0], "src/maps/map.json");
}

static char * north[] = {"north"};
static char * south[] = {"south"};
static char * east[] = {"east"};
static char * west[] = {"west"};
int worldMap_GetInput(struct Interface *ui, 
	struct WorldMap *map, struct InterfaceInput *input)
{
	char c = ui->ReadKey(ui);
	switch (c) {
	case 'l':
		command(ui, map, "look", NULL);
		return 1;
	case 'w':
		command(ui, map, "move", north);
		return 1;
	case 'a':
		command(ui, map, "move", west);
		return 1;
	case 's':
		command(ui, map, "move", south);
		return 1;
	case 'd':
		command(ui, map, "move", east);
		return 1;
	case 't':
		return console_read(ui, map, input);
	case 'q':
		return 0;
	default:
		ui->WriteLine(ui, "%c: Not a valid command!", c);
		return 1;
	}
	return 1;
}

void worldMap_Step(struct Interface *ui, 
	struct WorldMap *map, struct InterfaceInput *input)
{
}

void worldMap_Draw(struct Interface *ui, struct WorldMap *map)
{
	struct Map *m = &map->levels[0];
	struct Window *w = ui->game_win;
	int mapi, mapy, mapx;
	int centerx = (w->cols/2);
	int centery = (w->rows/2);
	for (int y = 0; y < w->rows; y++) {
		mapy = map->player->ypos - (w->rows/2) + y;
		for (int x = 0; x < w->cols; x++) {
			mapx = map->player->xpos - (w->cols/2) + x;
			mapi = mapy * m->cols + mapx;
			mvwaddch(w->win, y, x, 
				(mapi < 0 || mapi >= m->cols * m->rows) 
				|| (mapx < 0 || mapx >= m->cols)
				|| (mapy < 0 || mapy >= m->rows)
				? ' ' : m->map[mapi]->ch);
		}
	}
	mvwaddch(w->win, centery, centerx, map->player->ch);
	wrefresh(ui->game_win->win);
}

void worldMap_Shutdown(struct WorldMap *map)
{
	unload_commands();
	unload_level(&map->levels[0]);
	destroy_tiles();
}
