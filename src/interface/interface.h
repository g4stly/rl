#ifndef interface_h_
#define interface_h_

#include <ncurses.h>
#include "../list/list.h"

#define MAX_INPUT_LENGTH 256
#define ALT_KEYBACKSPACE 127

struct Window {
	int border;
	int rows;
	int cols;
	WINDOW *win;
};

struct InterfaceInput {
	char *content;
	size_t length;
};

struct Line {
	char content[MAX_INPUT_LENGTH];
	size_t length;
};

struct Interface {
	int rows;
	int cols;
	struct Window *game_win;	
	struct Window *status_win;
	struct Window *console_win;	
	struct ListNode *console_buffer; // struct Line pointers


	// <air-quotes>Methods</fake-html>
	void (*Move)(struct Window *w, int row, int col);
	char (*ReadKey)(struct Interface *i);
	void (*PutChar)(struct Interface *i, const char c);
	void (*PutString)(struct Interface *i, const char *fmt, ...);
	void (*WriteLine)(struct Interface *i, const char *fmt, ...);
	void (*ReadLine)(struct Interface *i, struct InterfaceInput *input);
};

void interface_Init(struct Interface *i);
void interface_Shutdown(struct Interface *i);

#endif
