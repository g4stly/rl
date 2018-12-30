#include <stdlib.h>
#include <string.h>
#include "../interface/interface.h"
#include "../list/list.h"
#include "../util.h"
#include "worldmap.h"

static struct ListNode *commands = NULL;

static int look_cmd(struct Interface *ui, struct WorldMap *m, char **argv)
{
	ui->WriteLine(ui, "You take a quick look around...");
	return 1;
}

static int echo_cmd(struct Interface *ui, struct WorldMap *m, char **argv)
{
	char *word = *argv;
	for (int i = 0; word; word = argv[++i]) {
		ui->WriteLine(ui, word);
	}
	return 1;
}

static int quit_cmd(struct Interface *ui, struct WorldMap *m, char **argv)
{
	return 0;
}

void command_try(struct Command *cmd, const char *word, char **words)
{
	// sorry in advance for the hacks

	// even though i apologized, there is
	// literally no reason why I shouldn't
	// just go with the hack here btw

	int tie = 0;
	uint64_t canidate[2] = { 0, 0};
	struct ListNode *current = commands;
	while (current) {
		if (!current->data) {
			current = current->next;
			continue;
		}

		struct Command *c = (struct Command*) current->data;
		unsigned int diff = string_compare(c->name, word, CMD_NAME_LEN);

		if (diff >= canidate[0]) {
			tie = (diff == canidate[0]) ? 1 : 0;
			canidate[0] = diff;
			canidate[1] = (uint64_t) c;
		}
		current = current->next;
	}

	if (!tie && canidate[0]) {
		struct Command *c = (struct Command *) canidate[1];
		cmd->func = c->func;
		memcpy(cmd->name, c->name, CMD_NAME_LEN - 1);
	}
}

int command(struct Interface *ui, struct WorldMap *m, const char *word, char **words)
{
	struct Command cmd;
	memset(&cmd, 0, sizeof(struct Command));
	command_try(&cmd, word, words);	
	if (!cmd.func) return 1;
	return cmd.func(ui, m, words);
}

static void mkcmd(const char *name, int (*fn)(struct Interface *, struct WorldMap *, char **))
{
	struct Command *leak = malloc(sizeof(struct Command));
	if (!leak) { die("malloc():"); }

	leak->func = fn;
	memset(leak->name, 0, CMD_NAME_LEN);
	memcpy(leak->name, name, CMD_NAME_LEN - 1);

	pushList(&commands, leak);
}

void load_commands(void)
{
	mkcmd("look", look_cmd);
	mkcmd("echo", echo_cmd);
	mkcmd("quit", quit_cmd);
}

void unload_commands(void)
{
	while (commands) {
		fpopList(&commands);
	}
}
