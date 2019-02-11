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

static void entity_move(struct Entity *self, struct WorldMap *m, char dir)
{
	int targetx = self->xpos;
	int targety = self->ypos;

	struct Map lvl = m->levels[m->player.zpos];

	switch (dir) {
	case 'n':
		targety -= 1;
		break;
	case 's':
		targety += 1;
		break;
	case 'e':
		targetx += 1;
		break;
	case 'w':
		targetx -= 1;
		break;
	}

	int targeti = targety * lvl.cols + targetx;
	if (targeti >= lvl.cols * lvl.rows || targeti < 0 
		|| targetx < 0 || targetx >= lvl.cols
		|| targety < 0 || targety >= lvl.rows)
	{ 
		return;
	}

	if (lvl.map[targeti]->wall) { return; }
	if (lvl.entity_layer[targeti]) { return; }
	if (targeti == m->player.ypos * lvl.cols + m->player.xpos) { return; }

	self->xpos = targetx;
	self->ypos = targety;
}

static int AI_wander(struct Entity *self, 
	struct Interface *ui, 
	struct WorldMap *m)
{
	switch(randomInt(16)) {
	case 12:
		ui->WriteLine(ui, "something is wandering nearby...");
		break;
	case 13:
		entity_move(self, m, 'n');
		break;
	case 14:
		entity_move(self, m, 'e');
		break;
	case 15:
		entity_move(self, m, 's');
		break;
	case 16:
		entity_move(self, m, 'w');
		break;
	}
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
	e->AI = AI_wander;
	pushList(entities, e);
}
