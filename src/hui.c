/* Copyright 2022 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <termios.h>

#include "config.h"

struct TermInfo {
	long unsigned width;
	long unsigned height;
	long unsigned x, y;
};

/* print function,
 * which counts the amount of new line characters
 * and line breaks due to terminal width
 */
void
hprint(struct TermInfo *term, const char *str)
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

void
draw_menu(
	struct TermInfo		*term,
	const char		*header,
	const struct Menu	 menu)
{
	long unsigned i;
	
	hprint(term, header);
	hprint(term, menu.title);
	hprint(term, "\n");

	/* TODO this only works as long as the menu struct is cleanly allocated
	 * find out if this could hinder portability
	 */
	for (i = 0; menu.entries[i].type != ET_NONE; i++) {
		hprint(term, "> ");
		hprint(term, MENU_MAIN.entries[i].caption);
		hprint(term, "\n");
	}

	for (i = 0; term->y < (term->height - 1); i++)
		hprint(term, "\n");

	printf(":");
}

int
main()
{	
	int active = -1;
	struct winsize wsize;
	struct termios orig, raw;
	struct TermInfo term;
	int key;
	char c;

	/* get term info and set raw mode */
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &wsize);
	term.width = wsize.ws_col;
	term.height = wsize.ws_row;
	term.x = 0;
	term.y = 0;

	tcgetattr(STDIN_FILENO, &orig);
	raw = orig;
	raw.c_lflag &= ~(ECHO | ICANON);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

	while (active) {
		draw_menu(&term, HEADER, MENU_MAIN);
		
		key = read(STDIN_FILENO, &c, 1);
	
		/* TODO that doesn't quit */
		if (key == 1) {
			if (c == 'q') {
				active = 0;
			}
		}
	}
	
	/* restore original terminal mode */
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig);

	return 0;
}

