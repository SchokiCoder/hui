/* Copyright (C) 2022 - 2023 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

/* This is the main file of the hui pager, called "courier".
 */

#include <stdio.h>

#include "common.h"
#include "config.h"

void
draw(long unsigned *stdout_y,
     const struct String *text,
     const long unsigned *scroll)
{
	long unsigned i,
	              text_x = 0,
		      text_y = 0;

	/* skip the text that is scrolled over */
	for (i = 0; i < text->len; i += 1) {
		if (text_x >= term_x_last)
			i -= 1;

		if (text_x >= term_x_last || '\n' == text->str[i]) {
			text_x = 0;
			text_y += 1;
		} else {
			text_x += 1;
		}

		if (text_y >= *scroll)
			break;
	}

	if (*scroll > 0)
		i += 1;

	/* print text */
	set_fg(READER_FG);
	set_bg(READER_BG);
	
	for (i = i; i < text->len; i += 1) {
		if (text_x >= term_x_last)
			i -= 1;

		if (text_x >= term_x_last || '\n' == text->str[i]) {
			fputc('\n', stdout);
			text_x = 0;
			text_y += 1;
			*stdout_y += 1;
		} else {
			fputc(text->str[i], stdout);
			text_x += 1;
		}

		if (*stdout_y >= term_y_last)
			break;
	}
}

void
handle_key(const char         key,
           int               *active,
           char              *cmdin,
           long unsigned     *cursor,
           const struct Menu *cur_menu,
           struct String     *feedback,
	   unsigned long     *feedback_lines,
	   enum InputMode    *imode,
           long unsigned     *scroll,
	   long unsigned     *text_lines)
{
	if (IM_CMD == *imode) {
		handle_key_cmdline(key,
				   cmdin,
				   active,
				   cursor,
				   cur_menu,
				   imode,
				   feedback,
				   feedback_lines);
		return;
	}
	
	switch (key) {
	case KEY_DOWN:
		if (*scroll < (*text_lines - 1))
			*scroll += 1;
		break;

	case KEY_UP:
		if (*scroll > 0)
			*scroll -= 1;
		break;

	case SIGINT:
	case SIGTSTP:
	case KEY_QUIT:
	case KEY_LEFT:
		active = 0;
		break;

	case KEY_CMD:
		*imode = IM_CMD;
		break;
	}
}

int main(int argc, char **argv)
{
	int            active = 1;
	char           c;
	struct String  feedback = String_new();
	long unsigned  feedback_lines = 0;
	enum InputMode imode = IM_NORMAL;
	long unsigned  scroll = 0;
	long unsigned  stdout_y;
	struct String  text = String_new();
	long unsigned  text_lines = 0;
	struct String  title;

	if (handle_cmdline_opts(argc,
				argv,
				"Courier",
				"courier",
				VERSION) != 0)
		return 0;

	get input;
	String_copy(&ardr->text, str);
	ardr->text_lines = str_lines(str, term_y_last);
	
	term_get_size();
	term_set_raw();
	
	while (active) {		
		draw_upper(&stdout_y, HEADER, title-as-cmdline-opt);
		draw_text(&stdout_y, &text, &scroll);
		draw_lower(cmdin, imode, &feedback);
	
		read(STDIN_FILENO, &c, 1);
		handle_key(c, &active, &imode, &text_lines, &scroll);
	}
	
	term_restore();
	return 0;
}
