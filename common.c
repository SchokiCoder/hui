/* Copyright (C) 2022 - 2023 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "common.h"
#include "config.h"
#include "hstring.h"
#include "sequences.h"

void
draw_lower(const char           *cmdin,
	   const struct String  *feedback,
	   const enum InputMode  imode,
	   const long unsigned   term_y_len)
{
	set_cursor(1, term_y_len);
	hprintf(CMDLINE_FG, CMDLINE_BG, CMD_PREPEND);

	if (IM_CMD == imode)
		hprintf(CMDLINE_FG, CMDLINE_BG, cmdin);
	else
		hprintf(FEEDBACK_FG, FEEDBACK_BG, feedback->str);
}

void draw_upper(const char          *header,
		long unsigned       *stdout_y,
		const char          *title,
		const long unsigned  term_x_len)
{
	set_cursor(1, 1);
	hprintf(HEADER_FG, HEADER_BG, header);
	hprintf(TITLE_FG, TITLE_BG, "%s\n", title);

	*stdout_y += str_lines(header, term_x_len);
	*stdout_y += str_lines(title, term_x_len);
}

void
set_feedback(struct String       *feedback,
             long unsigned       *feedback_lines,
	     const char          *str,
	     const long unsigned  term_y_len)
{
	String_copy(feedback, str);
	*feedback_lines = str_lines(str, term_y_len);
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
}

void term_restore()
{
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &previous_terminal_settings);
	printf(SEQ_CRSR_SHOW);
	printf(SEQ_FG_DEFAULT);
	printf(SEQ_BG_DEFAULT);
}
