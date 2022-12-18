/* Copyright 2022 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdarg.h>

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

/* formatted print wrapper around hprint
 */
void hprintf(struct TermInfo *term, const char *fmt, ...)
{
	char *buf;
	va_list ap;
	va_start(ap, fmt);
	
	sprintf(buf, fmt, ap);
	hprint(term, buf);
	
	va_end(ap);
}

int main()
{
	long unsigned i;
	struct winsize wsize;
	struct TermInfo term;

	/* get term size */
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &wsize);
	term.width = wsize.ws_width;
	term.height = wsize.ws_height;

	/* draw */
	hprintf(&term, "%s%s\n", HEADER, MENU_MAIN.title);

	/* TODO this only works as long as the menu struct is cleanly allocated
	 * find out if this could hinder portability
	 */
	for (i = 0; MENU_MAIN.entries[i].type != ET_NONE; i++)
		hprintf(&term, "> %s\n", MENU_MAIN.entries[i].caption);

	/* TODO i < (term_height - entry_count) */
	for (i = 0; i < 18; i++)
		hprint(&term, "\n");

	printf(":");

	return 0;
}

