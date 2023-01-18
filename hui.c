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

#define VERSION "0.1.0"

#define SIGINT  '\003'
#define SIGTSTP '\032'

static long unsigned term_x_last, term_y_last;

void move_cursor(const long unsigned x, const long unsigned y)
{
	printf("\033[%lu;%luH", y, x);
}


void strn_bleach(char *str, const long unsigned len)
{
	long unsigned i;
	
	for (i = 0; i < len; i++) {
		str[i] = '\0';
	}
}

void str_add_char(char *str, const char c)
{
	long unsigned len = strlen(str);
	
	str[len] = c;
	str[len + 1] = '\0';
}

/* printf with color setting */
void
hprintf(const struct Color  fg,
        const struct Color  bg,
        const char         *fmt,
        ...)
{
	va_list ap;
	va_start(ap, fmt);
	
	set_fg(fg);
	set_bg(bg);
	vfprintf(stdout, fmt, ap);
	
	va_end(ap);
}

void
draw_menu(const char          *header,
          const struct Menu   *menu,
	  const long unsigned  cursor,
	  const char          *feedback,
	  const char          *cmd_input)
{
	long unsigned i;
	
	/* draw bg colors */
	hprintf(OVERALL_BG, OVERALL_BG, SEQ_CLEAR);
	
	/* draw menu text */
	move_cursor(1, 1);
	hprintf(HEADER_FG, HEADER_BG, header);
	hprintf(TITLE_FG, TITLE_BG, "%s\n", menu->title);

	for (i = 0; menu->entries[i].type != ET_NONE; i++) {
		if (i == cursor) {
			hprintf(ENTRY_HOVER_FG, ENTRY_HOVER_BG,
			        "> %s\n", menu->entries[i].caption);
		}
		else {
			hprintf(ENTRY_FG, ENTRY_BG,
			        "> %s\n", menu->entries[i].caption);
		}
	}
	
	move_cursor(1, term_y_last);	
	hprintf(CMDLINE_FG, CMDLINE_BG, CMD_PREPEND);
	
	if (cmd_input != NULL && strlen(cmd_input) > 0) {
		hprintf(CMDLINE_FG, CMDLINE_BG, cmd_input);
	} else {
		if (feedback != NULL && strlen(feedback) > 0) {
			hprintf(FEEDBACK_FG, FEEDBACK_BG, feedback);
		}
	}
}

int main(int argc, char *argv[])
{
	struct winsize wsize;
	struct termios orig, raw;
	char c;
	
	int active = -1;
	enum InputMode imode = IM_NORMAL;
	char cmdin[CMD_IN_MAX_LEN] = "";
	long unsigned cursor = 0;
	const char *feedback = NULL;

	long unsigned menu_stack_len = 1;
	const struct Menu *menu_stack[MENU_STACK_SIZE] = {
		[0] = &MENU_MAIN
	};
	const struct Menu *cur_menu = &MENU_MAIN;
	
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

	while (active) {
		draw_menu(HEADER, cur_menu, cursor, feedback, cmdin);

		if (imode == IM_CMD) {
			printf(SEQ_CRSR_SHOW);
		} else {
			printf(SEQ_CRSR_HIDE);
		}

		/* key handling */
		read(STDIN_FILENO, &c, 1);
		
		if (imode == IM_CMD) {
			switch (c) {
			case '\n':
				// TODO cmd parsing will be here
				// TODO NO BREAK STATEMENT here
				
			case SIGINT:
			case SIGTSTP:
				strn_bleach(cmdin, CMD_IN_MAX_LEN);
				imode = IM_NORMAL;
				break;
			default:
				str_add_char(cmdin, c);
				break;
			}

			continue;
		}
		
		switch (c) {
		case 'q':
			active = 0;
			break;

		case 'j':
			if (cur_menu->entries[cursor + 1].type != ET_NONE)
				cursor++;
			break;

		case 'k':
			if (cursor > 0)
				cursor--;
			break;
		
		case 'l':
			if (cur_menu->entries[cursor].type == ET_SUBMENU) {
				cur_menu = cur_menu->entries[cursor].submenu;
				menu_stack[menu_stack_len] = cur_menu;
				menu_stack_len++;
				feedback = NULL;
				cursor = 0;
			}
			break;
		
		case 'h':
			if (menu_stack_len > 1) {
				menu_stack_len--;
				cur_menu = menu_stack[menu_stack_len - 1];
				feedback = NULL;
				cursor = 0;
			}
			break;
		
		case ':':
			imode = IM_CMD;
			break;
		
		case SIGINT:
		case SIGTSTP:
			active = 0;
			break;
		}
	}

	/* restore original terminal settings */
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig);
	printf(SEQ_CRSR_SHOW);
	printf(SEQ_FG_DEFAULT);
	printf(SEQ_BG_DEFAULT);

	return 0;
}

