#ifndef player_h_
#define player_h_

#include "../interface/interface.h"

struct Player {
	char ch;
	int color;
	char name[32];
	int xpos, ypos, zpos;
};

void player_Init(
	struct Interface *i,
	struct Player *p,
	int argc,
	char **argv);

#endif
