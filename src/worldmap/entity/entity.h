#ifndef entity_h_
#define entity_h_

#ifndef worldmap_h_
#include "../worldmap.h"
#endif

#define ENTITY_GHOST 0

struct Entity {
	char ch;
	int color;
	int xpos, ypos;
	void (*AI)(struct Entity *self,
		struct Interface *ui, 
		struct WorldMap *m);
};

void entity_Spawn(struct ListNode **entities, int entityID);

#endif
