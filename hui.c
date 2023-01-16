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

#define VERSION "0.0.1"

static long unsigned term_x_last, term_y_last;

void move_cursor(const long unsigned x, const long unsigned y)
{
	printf("\033[%lu;%luH", y, x);
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
	  const char          *feedback)
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
	hprintf(CMDLINE_FG, CMDLINE_BG, ":");
	
	if (feedback == NULL || strlen(feedback) <= 0)
		return;
	
	hprintf(FEEDBACK_FG, FEEDBACK_BG, feedback);
}

int main(int argc, char *argv[])
{
	struct winsize wsize;
	struct termios orig, raw;
	char c;
	
	int active = -1;
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
	raw.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
	
	printf(SEQ_CRSR_HIDE);

	while (active) {
		draw_menu(HEADER, cur_menu, cursor, feedback);

		/* key handling */
		read(STDIN_FILENO, &c, 1);
		
		switch (c) {
		case 'q':
			active = 0;
			break;

		case 'j':
			if (MENU_MAIN.entries[cursor + 1].type != ET_NONE)
				cursor++;
			break;

		case 'k':
			if (cursor > 0)
				cursor--;
			break;
		
		case 'l':
			if (cur_menu->entries[cursor].type == ET_SUBMENU) {
				feedback = NULL;
				cursor = 0;
				cur_menu = cur_menu->entries[cursor].submenu;
				menu_stack[menu_stack_len] = cur_menu;
				menu_stack_len++;
			}
			break;
		
		case 'h':
			if (menu_stack_len > 1) {
				feedback = NULL;
				cursor = 0;
				menu_stack_len--;
				cur_menu = menu_stack[menu_stack_len - 1];
			}
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

