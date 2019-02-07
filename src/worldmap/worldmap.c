#include <math.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "worldmap.h"
#include "tiles/tiles.h"
#include "entity/entity.h"
#include "../interface/interface.h"
#include "../util.h"


static int console_read(struct Interface *ui, struct WorldMap *m, struct InterfaceInput *input)
{
	ui->ReadLine(ui, input);
	if (input->content == NULL) {
		return 1;
	}
	ui->WriteLine(ui, input->content);
	int result = 0;
	char **words = split_string(input->content, ' ');
	char *word;

	while ((word = words[++result]));
	word = *words;

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

	// result holds argc
	result = cmd.func(ui, m, words, result);
	free_ptrArray(words);
	return result;
}

void worldMap_Init(struct WorldMap *map)
{
	init_tiles();
	load_commands();
	memset(map->levels, 0, sizeof(struct Map) * 5);

	map->player.ch = '@';
	map->player.color = REDBLACK;
	map->player.xpos = 1;
	map->player.ypos = 1;
	map->player.zpos = 0;

	load_level(&map->levels[0], "src/maps/map.json");
	fprintf(stderr, "world map initialized");
}

static char command_buffer = 0;	// LOL	// TODO: why is this funny?
static char *const move_literal = "move";
static char * north[] = {move_literal, "north"};
static char * south[] = {move_literal, "south"};
static char * east[] = {move_literal, "east"};
static char * west[] = {move_literal, "west"};
static int rl(struct Interface *ui,
	struct WorldMap *map, struct InterfaceInput *input, char c)
{
	switch (c) {
	case 'l':
		command(ui, map, "look", NULL, 0);
		break;
	case 'r':
		return rl(ui, map, input, command_buffer);
	case 'w':
		command(ui, map, north[0], north, 2);
		break;
	case 'a':
		command(ui, map, west[0], west, 2);
		break;
	case 's':
		command(ui, map, south[0], south, 2);
		break;
	case 'd':
		command(ui, map, east[0], east, 2);
		break;
	case 't':
		return console_read(ui, map, input);
	case 'q':
		return 0;
	default:
		if (c < 32) c = ' ';
		ui->WriteLine(ui, "%c: Not a valid command!", c);
		break;
	}
	if (c != 'r') command_buffer = c;
	return 1;
}

int worldMap_GetInput(struct Interface *ui, 
	struct WorldMap *map, struct InterfaceInput *input)
{
	return rl(ui, map, input, ui->ReadKey(ui));
}

void worldMap_Step(struct Interface *ui, 
	struct WorldMap *map, struct InterfaceInput *input)
{
}

void worldMap_Draw(struct Interface *ui, struct WorldMap *map)
{
	struct Map *m = &map->levels[map->player.zpos];
	struct Window *w = ui->game_win;
	
	// walk entities, keep track of index to draw them
	struct Entity *entities[m->rows * m->cols];
	memset(entities, '\0', sizeof(struct Entity *) * m->rows * m->cols);
	struct ListNode *current = m->entities;
	while (current) {
		struct Entity *e = (struct Entity *) current->data;
		int entityi = e->ypos * m->cols + e->xpos;
		if (entityi < m->rows * m->cols && entityi > 0) {
			entities[entityi] = e;
			fprintf(stderr, "added entity %i\n", entityi);
		}
		current = current->next;
	}

	char target;
	int target_color;
	int mapi, mapy, mapx;
	int centerx = (w->cols/2);
	int centery = (w->rows/2);
	for (int y = 0; y < w->rows; y++) {
		mapy = map->player.ypos - (w->rows/2) + y;
		for (int x = 0; x < w->cols; x++) {
			mapx = map->player.xpos - (w->cols/2) + x;
			mapi = mapy * m->cols + mapx;

			fprintf(stderr, "mapx: %i\nmapi: %i\n", mapx, mapi);
			if (mapi >= m->cols * m->rows || mapi < 0) {
				target = '?'; 
				target_color = REDBLACK;
			} else { 
				fprintf(stderr, "found\n");
				target = m->map[mapi]->ch;
				target_color = m->map[mapi]->color;
				if (entities[mapi]) { 
					target = entities[mapi]->ch; 
					target_color = entities[mapi]->color;
				}
			}

			fprintf(stderr, "color: %i\n", target_color);
			wattron(ui->game_win->win, COLOR_PAIR(target_color));
			mvwaddch(w->win, y, x, 
				(mapi < 0 || mapi >= m->cols * m->rows) 
				|| (mapx < 0 || mapx >= m->cols)
				|| (mapy < 0 || mapy >= m->rows)
				? ' ' : target);
		}
	}

	wattron(ui->game_win->win, COLOR_PAIR(map->player.color));
	mvwaddch(w->win, centery, centerx, map->player.ch);

	wrefresh(ui->game_win->win);
}

void worldMap_Shutdown(struct WorldMap *map)
{
	unload_commands();
	unload_level(&map->levels[0]);
	destroy_tiles();
}
