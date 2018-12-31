#include <jsmn.h>
#include <string.h>
#include <stdlib.h>
#include "worldmap.h"
#include "../list/list.h"
#include "../util.h"

#define FIELDS 7
#define ROWS 0
#define COLS 1
#define XPOS 2
#define YPOS 3
#define STRUCTURES 4
#define HOLLOW 5
#define TILE 6

#define FILE_SIZE 512
#define EMAPSYNTAX -4
#define EPARSEPM -5

struct level {
	int hollow;
	int rows, cols;
	int xpos, ypos;
	struct ListNode *structures;	// struct level pointers
};
static const char *const fields[] = { "rows", "cols", "posx", "posy", "structures", "hollow" };

static void load_failure(int status) {
	switch (status) {
	case EPARSEPM:	// parse post mortem
		die("Error: Parse Failed.\n");
		break;
	case EMAPSYNTAX:
		die("Invalid map syntax.\n");
		break;
	case JSMN_ERROR_INVAL:
		die("Invalid JSON.\n");
		break;
	case JSMN_ERROR_NOMEM:
		die("JSON string is too large.\n");
		break;
	case JSMN_ERROR_PART:
		die("Expecting more JSON data.\n");
		break;
	case 0: 
		die("Empty JSON.\n");
		break;
	default:
		die("Unknown error code: %i\n", status);
		break;
	}
}

static struct level *parse_level(const char *buffer, jsmntok_t *data, int *data_idx, int data_len)
{	// sorry in advance for the long lines
	if (data[*data_idx].type != JSMN_OBJECT) { load_failure(EMAPSYNTAX); }
	if (data[*data_idx].size < 6) { load_failure(EMAPSYNTAX); }

	struct level *l = malloc(sizeof(struct level));
	if (!l) { die("parse_level():"); }
	memset(l, '\0', sizeof(struct level));

	int proccessed = 0;
	int i = *data_idx + 1;
	for (i; proccessed < data[*data_idx].size; i+= 2) {
		int field = -1;
		for (int j = 0; j < FIELDS; j++) {
			int mismatch = strncmp(buffer+data[i].start, fields[j], strlen(fields[j]));
			if (!mismatch) { field = j; break; }
		}
		if (field < 0) { fprintf(stderr, "unexpected: %s\n", buffer+data[i].start); load_failure(EMAPSYNTAX); }
		switch (field) {
		case ROWS: 
			l->rows = atoi(buffer+data[i].start);
			break;
		case COLS: 
			l->cols= atoi(buffer+data[i].start);
			break;
		case XPOS: 
			l->xpos= atoi(buffer+data[i].start);
			break;
		case YPOS: 
			l->ypos= atoi(buffer+data[i].start);
			break;
		case STRUCTURES: 
			break;
		case HOLLOW: 
			l->hollow = (buffer[data[i].start] == 't' || 'T');
			break;
		}
	}

	return l;
}

void paint_level(struct Map *m, struct level *l)
{
	fprintf(stderr, "painting\n");
}

void load_level(struct Map *m, const char *filename)
{
	int buffer_len;
	char buffer[FILE_SIZE];
	FILE *file = fopen(filename, "r");
	if (!file) { die("load_level(): failed to open file %s:", filename); }
	buffer[fread(buffer, 1, FILE_SIZE - 1, file)] = '\0';
	buffer_len = strlen(buffer);
	fclose(file);

	jsmn_parser p;	
	jsmn_init(&p);

	// call parse with NULL to count # of tokens only
	int status = jsmn_parse(&p, buffer, buffer_len, NULL, 0);
	if (status <= 0) { load_failure(status); }

	jsmn_init(&p);
	jsmntok_t data[status];	// initialize to # of tokens
	status = jsmn_parse(&p, buffer, buffer_len, data, status);
	if (status <= 0) { load_failure(status); }

	int idx = 0;
	struct level *l = parse_level(buffer, data, &idx, status);
	paint_level(m, l);
}

#undef FIELDS
#undef ROWS
#undef COLS
#undef XPOS
#undef YPOS
#undef STRUCTURES
#undef HOLLOW

#undef FILE_SIZE
#undef EMAPSYNTAX
#undef EPARSEPM
