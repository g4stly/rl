#include <stdlib.h>
#include <stdio.h>

#ifndef entity_h_
#include "entity.h"
#endif

#ifndef util_h_
#include "../../util.h"
#endif

void entity_Rm(struct ListNode **entities, int entityId)
{
	if (!entities) { die("rm_entity(): null argument"); }
	struct Entity *e;
	struct ListNode *prev = NULL;
	struct ListNode *current = *entities;
	while (current) {
		e = (struct Entity *) current->data;
		if (!e) { die("rm_entity(): null entity"); }
		if (e->Id == entityId) { break; }
		prev = current;
		current = current->next;
	}
	if (!current) { die("rm_entity(): failed to rm entity #%i", entityId); }
	if (!prev) {
		*entities = (current->next) ? current->next : prev;
	} else {
		prev->next = current->next;
	}
	free(current->data);
	free(current);

}

static int AI_say_hello(struct Entity *self, 
	struct Interface *ui, 
	struct WorldMap *m)
{
	return 1;
}

static int atomicId = 1;
void entity_Spawn(struct ListNode **entities, char ch, int color, int xpos, int ypos)
{	// dealloc'd in unload_level()
	struct Entity *e = malloc(sizeof(struct Entity));
	if (!e) { die("malloc():"); }
	e->Id = atomicId++;
	e->ch = ch;
	e->color = color;
	e->xpos = xpos;
	e->ypos = ypos;
	e->con = 5;
	e->hp = 5;
	e->att = 5;
	e->def = 4;
	e->AI = AI_say_hello;
	pushList(entities, e);
}
