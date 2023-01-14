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

struct TermInfo {
	long unsigned width;
	long unsigned height;
	long unsigned x, y;
};

/* print function,
 * which counts the amount of new line characters
 * and line breaks due to terminal width
 */
void hprint(struct TermInfo *term, const char *str)
{
	long unsigned i;

	for (i = 0; str[i] != '\0'; i++) {
		putc(str[i], stdout);
		term->x++;

		if (term->x > term->width || str[i] == '\n') {
			term->x = 0;
			term->y++;
		}
	}
}

/* hprint with color setting */
void
hprintc(struct TermInfo    *term,
        const char         *str,
        const struct Color  fg,
        const struct Color  bg)
{
	set_fg(fg);
	set_bg(bg);
	hprint(term, str);
}

void
draw_menu(struct TermInfo     *term,
          const char          *header,
          const struct Menu   *menu,
	  const long unsigned  cursor)
{
	long unsigned i;

	hprintc(term, header, HEADER_FG, HEADER_BG);
	hprintc(term, menu->title, TITLE_FG, TITLE_BG);
	
	printf(SEQ_FG_DEFAULT);
	printf(SEQ_BG_DEFAULT);
	hprint(term, "\n");

	for (i = 0; menu->entries[i].type != ET_NONE; i++) {
		if (i == cursor) {
			set_fg(ENTRY_HOVER_FG);
			set_bg(ENTRY_HOVER_BG);
		}
		else {
			set_fg(ENTRY_FG);
			set_bg(ENTRY_BG);
		}
		
		hprint(term, "> ");
		hprint(term, menu->entries[i].caption);
		
		printf(SEQ_FG_DEFAULT);
		printf(SEQ_BG_DEFAULT);
		hprint(term, "\n");
	}

	for (i = 0; term->y < (term->height - 1); i++)
		hprint(term, "\n");

	// TODO this printf doesn't mess with cursor pos, so i keep it for now
	// this is a hprint later
	printf(":");
}

int main(int argc, char *argv[])
{
	int active = -1;
	struct winsize wsize;
	struct termios orig, raw;
	struct TermInfo term;
	char c;
	long unsigned cursor = 0;

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
	term.width = wsize.ws_col;
	term.height = wsize.ws_row;
	term.x = 0;
	term.y = 0;

	setbuf(stdout, NULL);
	tcgetattr(STDIN_FILENO, &orig);
	raw = orig;
	raw.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
	
	printf(SEQ_CRSR_HIDE);

	while (active) {
		/* reset term info, draw */
		term.x = 0;
		term.y = 0;
		puts(SEQ_CLEAR);
		draw_menu(&term, HEADER, cur_menu, cursor);

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
				cursor = 0;
				cur_menu = cur_menu->entries[cursor].submenu;
				menu_stack[menu_stack_len] = cur_menu;
				menu_stack_len++;
			}
			break;
		
		case 'h':
			if (menu_stack_len > 1) {
				cursor = 0;
				menu_stack_len--;
				cur_menu = menu_stack[menu_stack_len - 1];
			}
			break;
		}
	}

	/* restore original terminal mode */
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig);
	printf(SEQ_CRSR_SHOW);

	return 0;
}

