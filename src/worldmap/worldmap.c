#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include "worldmap.h"
#include "../interface/interface.h"
#include "../util.h"


static int console_read(struct Interface *ui, struct InterfaceInput *input)
{
	ui->ReadLine(ui, input);
	ui->WriteLine(ui, input->content);
	char **words = split_string(input->content, ' ');

	char *word = words[0];
	for (int i = 0; word; word = words[++i]) {
		ui->WriteLine(ui, word);	
		free(word);
	}

	free(words);
	return 1;
}



void worldMap_Init(struct WorldMap *map, struct Player *player)
{
	map->array[3] = 8;
	map->player = player;
}

int worldMap_GetInput(struct Interface *ui, 
	struct WorldMap *map, struct InterfaceInput *input)
{
	char c = ui->ReadKey(ui);
	switch (c) {
	case 'l':
		ui->WriteLine(ui, "You take a look around...");
		return 1;
	case 't':
		return console_read(ui, input);
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
