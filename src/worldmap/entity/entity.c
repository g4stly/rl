#include <stdlib.h>
#include <stdio.h>

#ifndef entity_h_
#include "entity.h"
#endif

#ifndef util_h_
#include "../../util.h"
#endif

static void AI_say_hello(struct Entity *self, 
	struct Interface *ui, 
	struct WorldMap *m)
{
	ui->WriteLine(ui, "Entity says hello, world!");
}

void entity_Spawn(struct ListNode **entities, int entityID)
{	// dealloc'd in unload_level()
	struct Entity *e = malloc(sizeof(struct Entity));
	if (!e) { die("malloc():"); }
	fprintf(stderr, "spawning\n");
	e->ch = 'E';
	e->xpos = 2;
	e->ypos = 3;
	e->AI = AI_say_hello;
	pushList(entities, e);
	fprintf(stderr, "spawned\n");
}
