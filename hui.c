/* Copyright 2022 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

#include "sequences.h"
#include "hstring.h"
#include "config.h"

enum InputMode {
	IM_NORMAL = 0,
	IM_CMD =    1
};

#define VERSION "0.2.0"

#define SIGINT  '\003'
#define SIGTSTP '\032'

static long unsigned term_x_last, term_y_last;

struct Runtime {
	int                  active;
	long unsigned        cursor;
	long unsigned        scroll;
	enum InputMode       imode;
	char                 cmdin[CMD_IN_MAX_LEN];
	const struct Menu   *cur_menu;
	long unsigned        menu_stack_len;
	const struct Menu   *menu_stack[MENU_STACK_SIZE];
	char                *feedback;
	char                *long_feedback; /* aka multi line feedback */
	long unsigned        long_feedback_lines;
};

void init_runtime(struct Runtime *rt)
{
	rt->active = -1;
	rt->imode = IM_NORMAL;
	rt->cmdin[0] = '\0';
	rt->cursor = 0;
	rt->scroll = 0;
	
	
	#warning TEST_VALUES_AHEAD
	rt->feedback = "feedback single";
	rt->long_feedback = "Swallowed shampoo,\nprobably gonna die.\nIt smelled like fruit,\nthat was a lie.";
	rt->long_feedback_lines = str_lines(rt->long_feedback, term_x_last);
	/* once an sub-app gives strings via stdout and stderr, they will be
	 * assigned to our char pointers ^
	 * so for testing i will just simulate this manually for now
	 */
	
	
	rt->menu_stack_len = 1;
	rt->menu_stack[0] = &MENU_MAIN;
	rt->cur_menu = &MENU_MAIN;
}

long unsigned count_menu_entries(const struct Menu *menu)
{
	long unsigned i;
	
	for (i = 0; menu->entries[i].type != ET_NONE; i++) {}
	
	return i;
}

void move_cursor(const long unsigned x, const long unsigned y)
{
	printf("\033[%lu;%luH", y, x);
}

void draw_upper(const char *header, const char *title)
{
	/* draw bg color (clear) */
	hprintf(OVERALL_BG, OVERALL_BG, SEQ_CLEAR);
	
	/* draw menu text */
	move_cursor(1, 1);
	hprintf(HEADER_FG, HEADER_BG, header);
	hprintf(TITLE_FG, TITLE_BG, "%s\n", title);
}

void draw_lower(const char *cmdin, const char *feedback)
{
	move_cursor(1, term_y_last);
	hprintf(CMDLINE_FG, CMDLINE_BG, CMD_PREPEND);
	
	if (cmdin != NULL && strlen(cmdin) > 0) {
		hprintf(CMDLINE_FG, CMDLINE_BG, cmdin);
	} else {
		if (feedback != NULL && strlen(feedback) > 0) {
			hprintf(FEEDBACK_FG, FEEDBACK_BG, feedback);
		}
	}
}

/* Draw the environment in which multi-line feedback is displayed.
 * Doesn't manipulate the runtime.
 */
void draw_reader(const char *header, const struct Runtime *rt)
{	
	const long unsigned long_feedback_len = strlen(rt->long_feedback);
	long unsigned i, x = 0, y = 0;
	int wrap_line = 0;
	
	draw_upper(header, rt->cur_menu->title);
	
	for (i = 0; i < long_feedback_len; i++) {			
		if (x >= term_x_last) {
			x = 0;
			y++;
			wrap_line = 1;
		}
		
		if (rt->long_feedback[i] == '\n')
				y++;
		
		if (y >= rt->scroll) {
			if (wrap_line)
				putc('\n', stdout);

			putc(rt->long_feedback[i], stdout);
		}
		
		wrap_line = 0;
		x++;
	}
	
	draw_lower(rt->cmdin, rt->feedback);
}

/* Doesn't manipulate the runtime.
 */
void draw_menu(const char *header, const struct Runtime *rt)
{
	long unsigned i;
	
	draw_upper(header, rt->cur_menu->title);

	for (i = 0; rt->cur_menu->entries[i].type != ET_NONE; i++) {
		if (i == rt->cursor) {
			hprintf(ENTRY_HOVER_FG, ENTRY_HOVER_BG,
			        "> %s\n", rt->cur_menu->entries[i].caption);
		}
		else {
			hprintf(ENTRY_FG, ENTRY_BG,
			        "> %s\n", rt->cur_menu->entries[i].caption);
		}
	}
	
	draw_lower(rt->cmdin, rt->feedback);
}

/* Manipulates the runtime depending on given command.
 */
void handle_command(const char *cmd, struct Runtime *rt)
{
	int n;
	long unsigned menu_len;
	
	if (strcmp(cmd, "q") == 0
	    || strcmp(cmd, "quit") == 0
	    || strcmp(cmd, "exit") == 0) {
		rt->active = 0;
	} else {
		n = atoi(rt->cmdin);
		
		if (n > 0) {
			menu_len = count_menu_entries(rt->cur_menu);
			if (n >= menu_len)
				rt->cursor = menu_len - 1;
			else
				rt->cursor = n - 1;
		} else {
			rt->feedback = "Command not recognised";
			return;
		}
	}
}

void menu_handle_key(const char key, struct Runtime *rt)
{
	switch (key) {
	case 'q':
		rt->active = 0;
		break;

	case 'j':
		if (rt->cur_menu->entries[rt->cursor + 1].type != ET_NONE)
			rt->cursor += 1;
		break;

	case 'k':
		if (rt->cursor > 0)
			rt->cursor -= 1;
		break;

	case 'l':
		if (rt->cur_menu->entries[rt->cursor].type == ET_SUBMENU) {
			rt->cur_menu = rt->cur_menu->entries[rt->cursor].submenu;
			rt->menu_stack[rt->menu_stack_len] = rt->cur_menu;
			rt->menu_stack_len += 1;
			rt->feedback = NULL;
			rt->cursor = 0;
		}
		break;

	case 'h':
		if (rt->menu_stack_len > 1) {
			rt->menu_stack_len -= 1;
			rt->cur_menu = rt->menu_stack[rt->menu_stack_len - 1];
			rt->feedback = NULL;
			rt->cursor = 0;
		}
		break;

	case ':':
		rt->imode = IM_CMD;
		break;

	case SIGINT:
	case SIGTSTP:
		rt->active = 0;
		break;
	}
}

void reader_handle_key(const char key, struct Runtime *rt)
{
	switch (key) {
	case 'q':
		rt->active = 0;
		break;

	case 'j':
		if (rt->scroll < rt->long_feedback_lines)
			rt->scroll += 1;
		break;

	case 'k':
		if (rt->scroll > 0)
			rt->scroll -= 1;
		break;
	
	case 'h':
		rt->long_feedback = NULL;
		break;

	case ':':
		rt->imode = IM_CMD;
		break;

	case SIGINT:
	case SIGTSTP:
		rt->active = 0;
		break;
	}
}

/* Manipulates the runtime depending on given key.
 * Returns:
 * 0 - nothing needs to be done by calling code
 * 1 - the mainloop should "continue"
 */
int handle_key(const char key, struct Runtime *rt)
{
	if (rt->imode == IM_CMD) {
		switch (key) {
		case '\n':
			handle_command(rt->cmdin, rt);
			
		case SIGINT:
		case SIGTSTP:
			strn_bleach(rt->cmdin, CMD_IN_MAX_LEN);
			rt->imode = IM_NORMAL;
			break;

		default:
			str_add_char(rt->cmdin, key);
			break;
		}

		return 1;
	}

	if (rt->long_feedback != NULL)
		reader_handle_key(key, rt);
	else
		menu_handle_key(key, rt);
	
	return 0;
}

int main(int argc, char *argv[])
{
	struct winsize wsize;
	struct termios orig, raw;
	char c;
	struct Runtime rt;
	const long unsigned prepend_len = strlen(CMD_PREPEND);
	
	/* get term size */
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &wsize);
	term_x_last = wsize.ws_col;
	term_y_last = wsize.ws_row;
	
	init_runtime(&rt);
	
	/* parse opts */
	if (argc == 2 && !strcmp("-v", argv[1])) {
		printf("hui: version " VERSION "\n");
		return 0;
	}

	/* get term info and set raw mode */
	setbuf(stdout, NULL);
	tcgetattr(STDIN_FILENO, &orig);
	raw = orig;
	raw.c_lflag &= ~(ECHO | ICANON | ISIG);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
	
	printf(SEQ_CRSR_HIDE);

	while (rt.active) {
		if (rt.long_feedback != NULL)
			draw_reader(HEADER, &rt);
		else
			draw_menu(HEADER, &rt);

		if (rt.imode == IM_CMD)
			printf(SEQ_CRSR_SHOW);
		else
			printf(SEQ_CRSR_HIDE);

		read(STDIN_FILENO, &c, 1);
		if (handle_key(c, &rt) == 1)
			continue;
	}

	/* restore original terminal settings */
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig);
	printf(SEQ_CRSR_SHOW);
	printf(SEQ_FG_DEFAULT);
	printf(SEQ_BG_DEFAULT);

	return 0;
}

