/* Copyright (C) 2022 - 2024 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include "common.h"
#include "config.h"
#include "hstring.h"
#include "sequences.h"

void
call_pager(struct String       *feedback,
	   long unsigned       *feedback_lines,
	   const long unsigned  term_y_len)
{
	struct String sh_pager = String_new();
	
	String_copy(&sh_pager, "printf \"", strlen("printf \""));
	String_append(&sh_pager, feedback->str, feedback->len);
	String_append(&sh_pager, "\" | ", strlen("\" | "));
	String_append(&sh_pager, PAGER, strlen(PAGER));
	
#ifdef PAGER_TITLE
	String_append(&sh_pager, " -t \"", strlen(" -t \""));
	String_append(&sh_pager, PAGER_TITLE, strlen(PAGER_TITLE));
	String_append(&sh_pager, "\"", strlen("\""));
#endif
	
	term_restore();
	
	system(sh_pager.str);
	
	term_set_raw();
	set_feedback(feedback, feedback_lines, "\0", term_y_len);
#ifndef	STRING_NOT_ON_HEAP
	String_free(&sh_pager);
#endif
}

void
draw_lower(const char           *cmdin,
	   const struct String  *feedback,
	   const long unsigned   feedback_lines,
	   const enum InputMode  imode,
	   const long unsigned   term_y_len)
{
	set_cursor(1, term_y_len);
	hprintf(CMDLINE_FG, CMDLINE_BG, CMD_PREPEND);

	if (IM_CMD == imode)
		hprintf(CMDLINE_FG, CMDLINE_BG, cmdin);
	else if (feedback_lines == 1)
		hprintf(FEEDBACK_FG, FEEDBACK_BG, feedback->str);
}

void draw_upper(const char          *header,
		const long unsigned  header_size,
		long unsigned       *stdout_y,
		const char          *title,
		const long unsigned  title_size,
		const long unsigned  term_x_len)
{
	set_cursor(1, 1);
	
	if (strlen(header) != 0) {
		hprintf(HEADER_FG, HEADER_BG, header);
		*stdout_y += strn_lines(header, header_size, term_x_len);
	}
	
	if (strlen(title) != 0) {
		hprintf(TITLE_FG, TITLE_BG, "%s\n", title);
		*stdout_y += strn_lines(title, title_size, term_x_len);
	}
}

void
set_feedback(struct String       *feedback,
             long unsigned       *feedback_lines,
	     const char          *str,
	     const long unsigned  term_y_len)
{
	String_copy(feedback, str, strlen(str));
	*feedback_lines = strn_lines(feedback->str, feedback->size, term_y_len);
}

void term_get_size(long unsigned *x, long unsigned *y)
{
	struct winsize wsize;
	
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &wsize);
	*x = wsize.ws_col;
	*y = wsize.ws_row;
}

void term_set_raw()
{
	struct termios raw;
	
	setbuf(stdout, NULL);
	tcgetattr(STDIN_FILENO, &previous_terminal_settings);
	raw = previous_terminal_settings;
	raw.c_lflag &= ~(ECHO | ICANON | ISIG);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
	printf(SEQ_CRSR_HIDE);
	is_term_raw = 1;
}

void term_restore()
{
	if (0 == is_term_raw)
		return;

	tcsetattr(STDIN_FILENO, TCSAFLUSH, &previous_terminal_settings);
	printf(SEQ_CRSR_SHOW);
	printf(SEQ_FG_DEFAULT);
	printf(SEQ_BG_DEFAULT);
}
