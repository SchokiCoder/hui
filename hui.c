/* Copyright 2022 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

#include "sequences.h"
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
	enum InputMode       imode;
	char                 cmdin[CMD_IN_MAX_LEN];
	const struct Menu   *cur_menu;
	long unsigned        menu_stack_len;
	const struct Menu   *menu_stack[MENU_STACK_SIZE];
	char                *feedback;
};

void init_runtime(struct Runtime *rt)
{
	rt->active = -1;
	rt->imode = IM_NORMAL;
	rt->cmdin[0] = '\0';
	rt->cursor = 0;
	rt->feedback = NULL;
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

/* zero out n characters of string
 */
void strn_bleach(char *str, const long unsigned len)
{
	long unsigned i;
	
	for (i = 0; i < len; i++) {
		str[i] = '\0';
	}
}

/* append char to string
 */
void str_add_char(char *str, const char c)
{
	long unsigned len = strlen(str);
	
	str[len] = c;
	str[len + 1] = '\0';
}

/* printf with color setting
 */
void
hprintf(const struct Color  fg,
        const struct Color  bg,
        const char         *fmt,
        ...)
{
	va_list valist;
	va_start(valist, fmt);
	
	set_fg(fg);
	set_bg(bg);
	vfprintf(stdout, fmt, valist);
	
	va_end(valist);
}

/* Doesn't manipulate the runtime.
 */
void draw_menu(const char *header, const struct Runtime *rt)
{
	long unsigned i;
	
	/* draw bg color (clear) */
	hprintf(OVERALL_BG, OVERALL_BG, SEQ_CLEAR);
	
	/* draw menu text */
	move_cursor(1, 1);
	hprintf(HEADER_FG, HEADER_BG, header);
	hprintf(TITLE_FG, TITLE_BG, "%s\n", rt->cur_menu->title);

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
	
	move_cursor(1, term_y_last);
	hprintf(CMDLINE_FG, CMDLINE_BG, CMD_PREPEND);
	
	if (rt->cmdin != NULL && strlen(rt->cmdin) > 0) {
		hprintf(CMDLINE_FG, CMDLINE_BG, rt->cmdin);
	} else {
		if (rt->feedback != NULL && strlen(rt->feedback) > 0) {
			hprintf(FEEDBACK_FG, FEEDBACK_BG, rt->feedback);
		}
	}
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
	
	return 0;
}

int main(int argc, char *argv[])
{
	struct winsize wsize;
	struct termios orig, raw;
	char c;
	struct Runtime rt;
	int ret_handle_key;
	
	init_runtime(&rt);
	
	/* parse opts */
	if (argc == 2 && !strcmp("-v", argv[1])) {
		printf("hui: version " VERSION "\n");
		return 0;
	}

	/* get term info and set raw mode */	
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &wsize);
	term_x_last = wsize.ws_col;
	term_y_last = wsize.ws_row;

	setbuf(stdout, NULL);
	tcgetattr(STDIN_FILENO, &orig);
	raw = orig;
	raw.c_lflag &= ~(ECHO | ICANON | ISIG);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
	
	printf(SEQ_CRSR_HIDE);

	while (rt.active) {
		draw_menu(HEADER, &rt);

		if (rt.imode == IM_CMD) {
			printf(SEQ_CRSR_SHOW);
		} else {
			printf(SEQ_CRSR_HIDE);
		}

		read(STDIN_FILENO, &c, 1);
		ret_handle_key = handle_key(c, &rt);
		
		if (ret_handle_key == 1)
			continue;
	}

	/* restore original terminal settings */
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig);
	printf(SEQ_CRSR_SHOW);
	printf(SEQ_FG_DEFAULT);
	printf(SEQ_BG_DEFAULT);

	return 0;
}

