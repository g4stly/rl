#include <ncurses.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include "interface.h"
#include "../list/list.h"
#include "../util.h"


static void window_destroy(struct Window *w)
{
	delwin(w->win);
	free(w);
}

static void window_print(struct Window *w, int row, int col, const char *output)
{
	mvwprintw(w->win, row, col, output);
}

static void window_clear(struct Window *w)
{
	wclear(w->win);
	if (w->border) { box(w->win, 0, 0); }
}

static void window_refresh(struct Window *w)
{
	wrefresh(w->win);
}

static struct Window *window_create(int height, 
	int width, int ypos, int xpos, int border)
{
	struct Window *w = malloc(sizeof(struct Window));
	if (!w) {
		die("malloc(): %v", errno);
	}

	w->win = newwin(height, width, ypos, xpos);
	w->rows = height;
	w->cols = width;
	w->border = 0;
	if (border) { w->border = 1; }

	window_clear(w);
	window_refresh(w);
	return w;
}

static char interface_readkey(struct Interface *i)
{
	return wgetch(i->console_win->win);
}


static void drawConsole(struct Interface *i)
{
	window_clear(i->console_win);

	int line_num = 1;
	struct ListNode _current, *current = &_current;
	current->next = i->console_buffer;
	while (current->next) {
		if (line_num > i->console_win->rows) {
			while (current->next) fpopList(&current->next);
			continue;
		}
		window_print(i->console_win,
			i->console_win->rows - (line_num++), 0,
			((struct Line *)current->next->data)->content);
		current = current->next;
	}

	window_refresh(i->console_win);
}

static void putstring(struct Interface *i,
	const char *fmt,
	va_list *ap)
{
	int input_length;

	// we rely on the list being fpop'd to free() this
	struct Line *line = malloc(sizeof(struct Line));
	if (!line) {
		mega_die(i,"malloc(): %v", errno);
	}
	memset(line->content, 0, MAX_INPUT_LENGTH);

	input_length = vsnprintf(line->content, MAX_INPUT_LENGTH, fmt, *ap);
	if (input_length > MAX_INPUT_LENGTH) {
		i->WriteLine(i, "Wow there cowboy, slow down!");
		free(line);
		return;
	}
	// we include the nullbyte in our length
	line->length = input_length + 1;
	pushList(&i->console_buffer, line);
}

static void interface_putstring(struct Interface *i, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	putstring(i, fmt, &ap);
	va_end(ap);
}

static void interface_writeline(struct Interface *i, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	putstring(i, fmt, &ap);
	va_end(ap);
	drawConsole(i);
}

static void interface_readline(struct Interface *i,
	struct InterfaceInput *input)
{
	size_t len = 8;
	int c, index = 0;

	char *buffer = malloc(len * sizeof(char));
	if (!buffer) {
		die("malloc(): %i", errno);
	}
	memset(buffer, 0, len);

	i->WriteLine(i, "");
	i->Move(i->console_win, i->console_win->rows - 1, 0);
	i->PutChar(i, '>');
	window_refresh(i->console_win);

	while ((c = i->ReadKey(i)) != '\n') {
		if (index >= len - 1) {	// out of window
			len = len * 2;
			buffer = realloc(buffer, len);
			if (!buffer) die("realloc(): %i", errno);
		}
		if (c == KEY_BACKSPACE || c == ALT_KEYBACKSPACE) { // backspace
			i->Move(i->console_win, i->console_win->rows - 1, index--);
			i->PutChar(i, ' ');
			i->Move(i->console_win, i->console_win->rows - 1, index + 1);
			continue;
		}
		if (len > MAX_INPUT_LENGTH) { break; }
		i->PutChar(i, c);
		buffer[index++] = (char) c;	
	}
	buffer[index] = '\0';
	len = strlen(buffer);
	if (len < 1) {
		input->content = NULL;
		return;
	}

	input->length = len + 1; // include null byte
	input->content = malloc(input->length*sizeof(char));
	if (!input->content) {
		die("malloc(): %i", errno);
	}
	memcpy(input->content, buffer, input->length);
	fpopList(&i->console_buffer);
	free(buffer);
}


static void interface_putchar(struct Interface *i, const char output)
{
	waddch(i->console_win->win, output);
}

static void interface_move(struct Window *w, int row, int col)
{
	wmove(w->win, row, col);
}


void interface_Init(struct Interface *i)
{
	// initialize ncurses
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	keypad(stdscr, true);

	// do the colors
	start_color();
	init_pair(WHITEBLACK, COLOR_WHITE, COLOR_BLACK);
	init_pair(REDBLACK, COLOR_RED, COLOR_BLACK);
	init_pair(GREENBLACK, COLOR_GREEN, COLOR_BLACK);
	init_pair(YELLOWBLACK, COLOR_YELLOW, COLOR_BLACK);
	init_pair(BLUEBLACK, COLOR_BLUE, COLOR_BLACK);
	init_pair(MAGENTABLACK, COLOR_MAGENTA, COLOR_BLACK);
	init_pair(CYANBLACK, COLOR_CYAN, COLOR_BLACK);

	init_pair(BLACKBLUE, COLOR_BLACK, COLOR_BLUE);

	// create windows
	int rows, cols;
	getmaxyx(stdscr, rows, cols);
	int game_win_rows = (rows / 4) * 3;
	if (!game_win_rows % 2) game_win_rows++;
	int game_win_cols = game_win_rows * 2;

	i->game_win = window_create(
		game_win_rows, 
		game_win_cols, 
		0, 0, 0);

	i->status_win = window_create(
		game_win_rows, 
		cols - game_win_cols, 
		0, game_win_cols, 1);

	i->console_win = window_create(
		rows - game_win_rows,
		cols,
		game_win_rows, 0, 0);
	i->console_buffer = NULL;

	// register functions (dep injection)
	i->Move		= &interface_move;
	i->ReadKey 	= &interface_readkey;
	i->PutChar 	= &interface_putchar;
	i->PutString	= &interface_putstring;
	i->WriteLine 	= &interface_writeline;
	i->ReadLine	= &interface_readline;
}

void interface_Shutdown(struct Interface *i)
{
	// fpop our lists
	while (i->console_buffer) { fpopList(&i->console_buffer); }
	// free ncurses resources
	window_destroy(i->game_win);
	window_destroy(i->status_win);
	window_destroy(i->console_win);

	endwin();
}
