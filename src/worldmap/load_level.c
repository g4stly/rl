#include <jsmn.h>
#include <string.h>
#include <stdlib.h>
#include "worldmap.h"
#include "tiles/tiles.h"
#include "entity/entity.h"
#include "../list/list.h"
#include "../util.h"

/*
 * when you add new fields to the level struct you need to
 * 1) add it's identifier to the [fields] array
 * 2) define it's identifier in caps as it's index in the [fields] array 
 * 3) handle it's appearance in [parse_level()]
 * 	3.a) at the beginning of the case, [*data_idx] will be set to 
 *	the index of the KEY we are switching on.
 *	3.a) at the end of the case, after you've parsed the value, 
 *	[*data_idx] MUST point to <index of the next key to be parsed> - 2!
 *	This means it only needs to be incremented if we're parsing 
 *	as compound value.
 */

#define FIELDS 7
#define ROWS 0
#define COLS 1
#define XPOS 2
#define YPOS 3
#define STRUCTURES 4
#define HOLLOW 5
#define TILE 6

#define FILE_SIZE 2048
#define EMAPSYNTAX -4
#define EPARSEPM -5
#define EMAPVIOLATION -6
#define ETILETYPE -7
#define EMISREQVAL -8

struct level {
	int rows, cols;
	int xpos, ypos;
	int tile, hollow;
	struct ListNode *structures;	// struct level pointers
};
static const char *const fields[] = { "rows", "cols", "xpos", "ypos", "structures", "hollow", "tile"};

static void load_failure(int status) {
	switch (status) {
	case EPARSEPM:	// parse post mortem
		die("Error: Parse Failed.\n");
		break;
	case EMAPSYNTAX:
		die("Invalid map syntax.\n");
		break;
	case EMAPVIOLATION:
		die("Invalid map values.\n");
		break;
	case ETILETYPE:
		die("Unknown Tile Type.\n");
		break;
	case EMISREQVAL:
		die("Missing required value\n");
		break;
	case JSMN_ERROR_INVAL:
		die("Invalid JSON.\n");
		break;
	case JSMN_ERROR_NOMEM:
		die("JSON string is too large.\n");
		break;
	case JSMN_ERROR_PART:
		die("Expecting more JSON data; map file may exceed filesize limit.\n");
		break;
	case 0: 
		die("Empty JSON.\n");
		break;
	default:
		die("Unknown error code: %i\n", status);
		break;
	}
}

static void fill_tile(const char *buffer, const jsmntok_t *const token, int *tiletype)
{
	fprintf(stderr,"fill_tile()\n\t");
	*tiletype = -1;
	for (int i = 0; i < TILES; i++) {
		char *b = malloc(5);
		memcpy(b, buffer+token->start, 4);
		b[4] = '\0';
		fprintf(stderr,"before %s\n\t", tile_names[i]);
		int mismatch = strncmp(buffer+token->start, tile_names[i], 4);
		fprintf(stderr,"strncmp(%s, %s) = %i\n\t",b, tile_names[i], mismatch);
		if (!mismatch) { *tiletype = i; break; }
	}
	fprintf(stderr, "result: %i\n", *tiletype);
	if (*tiletype < 0) { load_failure(ETILETYPE); }
}

static void fill_levels(const char *buffer, jsmntok_t *data, int *data_idx, struct ListNode **levels);

static struct level *parse_level(const char *buffer, jsmntok_t *data, int *data_idx)
{	// sorry in advance for the long lines
	if (data[*data_idx].type != JSMN_OBJECT) { load_failure(EMAPSYNTAX); }
	fprintf(stderr, "arguments validated\n");

	struct level *l = malloc(sizeof(struct level));
	if (!l) { die("parse_level():"); }
	memset(l, '\0', sizeof(struct level));
	fprintf(stderr, "initialized level\n");

	int set[FIELDS];
	memset(set, 0, FIELDS * sizeof(int));
	int proccessed = 0;
	int size = data[(*data_idx)++].size;
	fprintf(stderr, "read %i fields\n", size);
	for (data_idx; proccessed < size; (*data_idx) += 2) {
		fprintf(stderr, "parsing #%i - parsed %i(%i)\n", *data_idx, proccessed, size);
		int field = -1;
		for (int j = 0; j < FIELDS; j++) {
			int mismatch = strncmp(buffer+data[*data_idx].start, fields[j], 4);
			if (!mismatch) { field = j; break; }
		}
		fprintf(stderr,"field: %i\n", field);
		switch (field) {
		case ROWS: 
			l->rows = atoi(buffer+data[*data_idx + 1].start);
			proccessed++;
			set[ROWS] = 1;
			break;
		case COLS: 
			l->cols= atoi(buffer+data[*data_idx + 1].start);
			proccessed++;
			set[COLS] = 1;
			break;
		case XPOS: 
			l->xpos= atoi(buffer+data[*data_idx + 1].start);
			proccessed++;
			set[XPOS] = 1;
			break;
		case YPOS: 
			l->ypos= atoi(buffer+data[*data_idx + 1].start);
			proccessed++;
			set[YPOS] = 1;
			break;
		case STRUCTURES: 
			/* after fill levels we're (hopefully) pointing to the next token */
			(*data_idx)++;	// here's to a safe return
			fill_levels(buffer, data, data_idx, &l->structures);
			(*data_idx) -= 2; 
			proccessed++;
			set[STRUCTURES] = 1;
			break;
		case HOLLOW: 
			l->hollow = buffer[data[*data_idx + 1].start] == 't'; 
			l->hollow = l->hollow || buffer[data[*data_idx + 1].start] == 'T';
			proccessed++;
			set[HOLLOW] = 1;
			break;
		case TILE:
			fill_tile(buffer, &data[*data_idx + 1], &l->tile);
			proccessed++;
			set[TILE] = 1;
			break;
		}
	}
	if (!set[ROWS] || !set[COLS] || !set[TILE]) { load_failure(EMISREQVAL); }
	return l;
}

static void fill_levels(const char *buffer, jsmntok_t *data, int *data_idx, struct ListNode **levels)
{
	if (data[*data_idx].type != JSMN_ARRAY) { load_failure(EMAPSYNTAX); }
	fprintf(stderr, "fill validated\n");

	struct level *l;
	int size = data[(*data_idx)++].size;
	fprintf(stderr, "fill %i levels\n", size);
	for (int i = 0; i < size; i++) {
		l = parse_level(buffer, data, data_idx);
		pushList(levels, l);
		fprintf(stderr, "next idx: %i\n", *data_idx);
	}
}

static void paint_walls(struct Map *m, struct level *l, int xorigin, int yorigin)
{
	yorigin = yorigin + l->ypos;
	xorigin = xorigin + l->xpos;
	for (int y = yorigin; y < yorigin + l->rows; y++) {
		m->map[y * m->cols + xorigin] = tiles[l->tile];
		m->map[y * m->cols + xorigin + (l->cols - 1)] = tiles[l->tile];
	}
	for (int x = xorigin; x < xorigin + l->cols; x++) {
		m->map[yorigin * m->cols + x] = tiles[l->tile];
		m->map[(yorigin + (l->rows - 1)) * m->cols + x] = tiles[l->tile];
	}
}

static void paint_block(struct Map *m, struct level *l, int xorigin, int yorigin)
{
	yorigin = yorigin + l->ypos;
	xorigin = xorigin + l->xpos;
	for (int y = yorigin; y < yorigin + l->rows; y++) {
		for (int x = xorigin; x < xorigin + l->cols; x++) {
			m->map[y * m->cols + x] = tiles[l->tile];
		}
	}
}

static void paint_level(struct Map *m, struct level *l, int xorigin, int yorigin)
{
	if (l->hollow) { paint_walls(m, l, xorigin, yorigin); }
	else { paint_block(m, l, xorigin, yorigin); }
	for (struct ListNode *n = l->structures; n; n = n->next) {
		if (!n->data) { continue; }
		paint_level(m, n->data, xorigin + l->xpos, yorigin + l->ypos);
	}
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
	jsmntok_t data[status];	// initialize to # of tokens (( TODO: how does the compiler do this?? ))
	status = jsmn_parse(&p, buffer, buffer_len, data, status);
	if (status <= 0) { load_failure(status); } int idx = 0;
	struct level *l = parse_level(buffer, data, &idx);

	l->xpos = 0;
	l->ypos = 0;
	m->rows = l->rows;
	m->cols = l->cols;

	/* freed unload_level() */
	m->map = malloc(sizeof(struct Tile *) * m->cols * m->rows);
	if (!m->map) { die("paint_level(): malloc():"); }

	/* freed unload_level() */
	m->entity_layer = malloc(sizeof(struct Entity *) * m->cols * m->rows);
	if (!m->entity_layer) { die("paint_level(): malloc():"); }

	paint_level(m, l, l->xpos, l->ypos);

	// entities
	// TODO: allow specification of entitiy location in map
	//fprintf(stderr, "spawning entities\n");
}

void unload_level(struct Map *m)
{
	free(m->map); 		/* malloc()'d load_level() */
	free(m->entity_layer); 	/* malloc()'d load_level() */
	fpopList(&m->entities);	/* malloc()'d entity_Spawn() */
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
