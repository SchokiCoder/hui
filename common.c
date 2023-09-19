/* Copyright (C) 2022 - 2023 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#include <stdio.h>

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

void handle_key_cmdline(const char key)
{
	switch (key) {
		case '\n':
			handle_command(cmd,
			               active,
				       cur_menu,
				       cursor,
				       feedback,
				       feedback_lines);
			/* fall through */
		case SIGINT:
		case SIGTSTP:
			strn_bleach(cmd, CMD_IN_LEN);
			*imode = IM_NORMAL;
			break;

		default:
			str_add_char(cmd, key);
			break;
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
		}
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

