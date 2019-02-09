#ifndef entity_h_
#define entity_h_

#ifndef worldmap_h_
#include "../worldmap.h"
#endif

struct Entity {
	char ch;
	int color;
	int xpos, ypos;
	int Id, con, hp, att, def;
	int (*AI)(struct Entity *self,
		struct Interface *ui, 
		struct WorldMap *m);
};

void entity_Spawn(struct ListNode **entities, char ch, int color, int xpos, int ypos);
void entity_Rm(struct ListNode **entities, int entityId);

#endif
