#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "worldmap.h"
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



void worldMap_Init(struct WorldMap *map, struct Player *player)
{
	load_commands();
	map->array[3] = 8;
	map->player = player;
}

int worldMap_GetInput(struct Interface *ui, 
	struct WorldMap *map, struct InterfaceInput *input)
{
	char c = ui->ReadKey(ui);
	switch (c) {
	case 'l':
		command(ui, "look", NULL);
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

void worldMap_Shutdown(void)
{
	unload_commands();
}
