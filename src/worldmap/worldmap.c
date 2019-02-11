#include <time.h>
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



static int exec_string(struct Interface *ui, struct WorldMap *m, const char *const input)
{
	int result = 0;
	char **words = split_string(input, ' ');
	char *word;

	while ((word = words[++result]));	// bad fucking ass
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

static int console_read(struct Interface *ui, struct WorldMap *m, struct InterfaceInput *input)
{
	ui->ReadLine(ui, input);
	if (input->content == NULL) {
		return 1;
	}
	ui->WriteLine(ui, input->content);
	return exec_string(ui, m, input->content);
}

void worldMap_Init(struct WorldMap *map)
{
	srand(time(NULL));
	init_tiles();
	load_commands();
	memset(map->levels, 0, sizeof(struct Map) * 5);

	map->time = 0;

	map->player.ch = '@';
	map->player.color = REDBLACK;
	map->player.xpos = 72;
	map->player.ypos = 22;
	map->player.zpos = 0;
	map->player.con = 10;
	map->player.hp = 10;
	map->player.att = 5;
	map->player.def = 5;
	map->player.dex = 5;
	map->player.luck = 5;

	load_level(&map->levels[0], "src/maps/map.json");
	fprintf(stderr, "world map initialized");
}

static int rl(struct Interface *ui,
	struct WorldMap *map, struct InterfaceInput *input, char c)
{
	char *buf;
	switch (c) {
	case 'l':
		return command(ui, map, "look", NULL, 0);
	case 'p': 
		buf = malloc(sizeof(char) * 32);
		if (!buf) { die("rl(): malloc():"); }
		snprintf(buf, 32, "spawn E %i %i", map->player.xpos, map->player.ypos);
		int rv = exec_string(ui, map, buf);
		free(buf);
		return rv;
	case 'w':
		return exec_string(ui, map, "move north");
	case 'a':
		return exec_string(ui, map, "move west");
	case 's':
		return exec_string(ui, map, "move south");
	case 'd':
		return exec_string(ui, map, "move east");
	case 't':
		return console_read(ui, map, input);
	case 'q':
		return 0;
	default:
		if (c < 32) c = ' ';
		ui->WriteLine(ui, "%c: Not a valid command!", c);
		break;
	}
	return 1;
}

int worldMap_GetInput(struct Interface *ui, 
	struct WorldMap *map, struct InterfaceInput *input)
{
	return rl(ui, map, input, ui->ReadKey(ui));
}

void worldMap_Step(struct Interface *ui, struct WorldMap *map, 
	struct InterfaceInput *input, int cmdResult)
{
	struct Map *m = &map->levels[map->player.zpos];

	if (cmdResult > 0) { map->time++; }
	memset(m->entity_layer, '\0', sizeof(struct Entity *) * m->cols * m->rows);

	int entityi;
	struct ListNode *current = m->entities;
	while (current) {
		struct Entity *e = (struct Entity *) current->data;
		if (!e) { die("NULL entity!"); }
		if (cmdResult > 0) {
			if (e->hp <= 0) { 
				entity_Rm(&m->entities, e->Id); 
				goto next;
			}
			if (e->AI) { e->AI(e, ui, map); }
		}

		entityi = e->ypos * m->cols + e->xpos;
		if (entityi < m->rows * m->cols && entityi > 0) {
			m->entity_layer[entityi] = e;
		}
		next:
		current = current->next;
	}

}

static void draw_stats(struct Interface *ui, struct WorldMap *map) {
	struct Map *m = &map->levels[map->player.zpos];
	struct Window *w = ui->status_win;
	
	mvwprintw(w->win, 1, 1, "HP: (%i/%i)", map->player.hp, map->player.con);
	mvwprintw(w->win, 2, 1, "att: %i\tdef: %i", map->player.att, map->player.def);
	mvwprintw(w->win, 3, 1, "dex: %i\tluck: %i", map->player.dex, map->player.luck);
	mvwprintw(w->win, 4, 1, "X: %i\tY: %i", map->player.xpos, map->player.ypos);
	wrefresh(w->win);
}

void worldMap_Draw(struct Interface *ui, struct WorldMap *map)
{	// TODO: split this function apart?
	struct Map *m = &map->levels[map->player.zpos];
	struct Window *w = ui->game_win;

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

			if ((mapi < m->cols * m->rows && mapi >= 0)
				&& (mapx >= 0 || mapx < m->cols) 
				&& (mapy >= 0 || mapy < m->rows))
			{	// TODO: more elegance
				target = m->map[mapi]->ch;
				target_color = m->map[mapi]->color;
				if (m->entity_layer[mapi]) { 
					target = m->entity_layer[mapi]->ch; 
					target_color = m->entity_layer[mapi]->color;
				}
			} else {
				target = ' ';
				target_color = WHITEBLACK;
			}

			wattron(w->win, COLOR_PAIR(target_color));
			mvwaddch(w->win, y, x, target);
		}
	}

	wattron(w->win, COLOR_PAIR(map->player.color));
	mvwaddch(w->win, centery, centerx, map->player.ch);

	wrefresh(w->win);

	draw_stats(ui, map);
}

void worldMap_Shutdown(struct WorldMap *map)
{
	unload_commands();
	unload_level(&map->levels[0]);
	destroy_tiles();
}
