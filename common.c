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
handle_cmd(const char          *cmdin,
	   int                 *active,
	   const struct Menu   *cur_menu,
	   long unsigned       *cursor,
	   struct String       *feedback,
	   unsigned long       *feedback_lines,
	   const long unsigned  term_y_len)
{
	long long n;
	long unsigned menu_len;

	if (strcmp(cmdin, "q") == 0
	    || strcmp(cmdin, "quit") == 0
	    || strcmp(cmdin, "exit") == 0) {
		*active = 0;
	} else {
		n = atoll(cmdin);

		if (n > 0) {
			menu_len = count_menu_entries(cur_menu);
			if ((unsigned long) n >= menu_len)
				*cursor = menu_len - 1;
			else
				*cursor = n - 1;
		} else {
			set_feedback(feedback,
                                     feedback_lines,
				     "Command not recognised",
				     term_y_len);
			return;
		}
	}
}

void handle_key_cmdline(const char           key,
			char                *cmdin,
			int                 *active,
			long unsigned       *cursor,
			const struct Menu   *cur_menu,
			enum InputMode      *imode,
			struct String       *feedback,
			unsigned long       *feedback_lines,
			const long unsigned  term_y_len)
{
	switch (key) {
		case '\n':
			handle_cmd(cmdin,
			           active,
				   cur_menu,
				   cursor,
				   feedback,
				   feedback_lines,
				   term_y_len);
			/* fall through */
		case SIGINT:
		case SIGTSTP:
			strn_bleach(cmdin, CMD_IN_LEN);
			*imode = IM_NORMAL;
			break;

		default:
			str_add_char(cmdin, key);
			break;
	}
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
