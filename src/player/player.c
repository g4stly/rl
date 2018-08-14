#include <string.h>
#include "player.h"

void player_Init(struct Interface *i,
	struct Player *p, int argc, char **argv)
{
	char *name = "Elizabeth";
	memcpy(p->name, name, strlen(name)+1);
}
