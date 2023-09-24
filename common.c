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
#include "license_str.h"
#include "sequences.h"

void
draw_lower(const char           *cmdin,
           const enum InputMode  imode,
           const struct String  *feedback)
{
	set_cursor(1, term_y_last);
	hprintf(CMDLINE_FG, CMDLINE_BG, CMD_PREPEND);

	if (IM_CMD == imode)
		hprintf(CMDLINE_FG, CMDLINE_BG, cmdin);
	else
		hprintf(FEEDBACK_FG, FEEDBACK_BG, feedback->str);
}

void draw_upper(long unsigned *stdout_y, const char *header, const char *title)
{
	set_cursor(1, 1);
	hprintf(HEADER_FG, HEADER_BG, header);
	hprintf(TITLE_FG, TITLE_BG, "%s\n", title);

	*stdout_y += str_lines(header, term_x_last);
	*stdout_y += str_lines(title, term_x_last);
}

void
handle_cmd(const char        *cmdin,
           int               *active,
           const struct Menu *cur_menu,
	   long unsigned     *cursor,
           struct String     *feedback,
	   unsigned long     *feedback_lines)
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
				     "Command not recognised");
			return;
		}
	}
}

int
handle_cmdline_opts(int          argc,
		    const char **argv,
		    const char  *app_name,
		    const char  *app_shortname,
		    const char  *app_version)
{
	if (2 == argc) {
		switch (argv[1][1]) {
		case 'v':
			printf("%s: version %s\n", app_shortname, app_version);
			return 1;
			break;

		case 'a':
			printf("\"%s\" aka %s %s is "
			       "licensed under the BSD-3-Clause license.\n"
			       "You should have received a copy of the license "
			       "along with this program.\n\n"
			       "The source code of this program is available "
			       "at:"
			       "\nhttps://github.com/SchokiCoder/hui\n\n"
			       "If you did not receive a copy of the license, "
			       "see below:\n\n"
			       "%s\n\n%s\n\n%s\n",
			       app_name, app_shortname, app_version,
			       MSG_COPYRIGHT, MSG_CLAUSES, MSG_WARRANTY);
			return 1;
			break;
		
		default:
			return 0;
		}
	}
	
	return 0;
}

void handle_key_cmdline(const char         key,
			char              *cmdin,
			int               *active,
			enum InputMode     *imode,
			const struct Menu *cur_menu,
			long unsigned     *cursor,
			struct String     *feedback,
			unsigned long     *feedback_lines)
{
	switch (key) {
		case '\n':
			handle_cmd(cmdin,
			           active,
				   cur_menu,
				   cursor,
				   feedback,
				   feedback_lines);
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
set_feedback(struct String *feedback,
             long unsigned *feedback_lines,
	     const char    *str)
{
	String_copy(feedback, str);
	*feedback_lines = str_lines(str, term_y_last);
}

void term_get_size()
{
	struct winsize wsize;
	
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &wsize);
	term_x_last = wsize.ws_col;
	term_y_last = wsize.ws_row;
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
