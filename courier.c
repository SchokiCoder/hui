/* Copyright (C) 2022 - 2023 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

/* This is the main file of the hui pager, called "courier".
 */

#include <stdio.h>

#include "common.h"
#include "config.h"
#include "license_str.h"

void
draw_text(long unsigned *stdout_y,
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

int
handle_cmdline_opts(const int argc, const char **argv, struct String *title)
{
	if (2 == argc) {
		switch (argv[1][1]) {
		case 'v':
			printf("%s: version %s\n", "courier", VERSION);
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
			       "Courier", "courier", VERSION,
			       MSG_COPYRIGHT, MSG_CLAUSES, MSG_WARRANTY);
			return 1;
			break;
		
		case 't':
			String_copy(title, argv[2]);
			return 0;
			break;
		
		default:
			return 0;
		}
	}
	
	return 0;
}

void
handle_key(const char         key,
           int               *active,
           char              *cmdin,
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

void read_target_file(FILE *file, struct String *text)
{
	char          buf[STRING_BLOCK_SIZE];
	long unsigned buf_len;
	int           read = 1;
	
	while (read) {
		buf_len = fread(buf, sizeof(char), STRING_BLOCK_SIZE, file);
		if (buf_len < STRING_BLOCK_SIZE)
			read = 0;

		String_append(text, buf, buf_len);
	}
}

/* First argument not starting with a dash, is used as path,
 * except a -t is given before that.
 */
FILE* open_target_file(const int argc, const char **argv)
{
	long unsigned i;
	
	for (i = 1; i < argc; i++) {
		if (argv[i][0] != '-')
			break;
		else if (argv[i][1] == 't')
			i++;
	}
	
	return fopen(argv[i], "r");
}

int main(int argc, char **argv)
{
	int             active = 1;
	char            c;
	char            cmdin[CMD_IN_LEN] = "\0";
	FILE           *target_file;
	struct String   feedback = String_new();
	long unsigned   feedback_lines = 0;
	enum InputMode  imode = IM_NORMAL;
	long unsigned   scroll = 0;
	long unsigned   stdout_y;
	struct String   text = String_new();
	long unsigned   text_lines = 0;
	struct String   title;

	if (handle_cmdline_opts(argc, argv, &title) != 0)
		return 0;
	
	target_file = open_target_file(argc, argv);
	if (NULL == target_file) {
		fprintf(stderr, "File could not be opened.");
		return 1;
	}

	read_target_file(target_file, &text);
	fclose(target_file);
	
	term_get_size();
	term_set_raw();
	
	text_lines = str_lines(text.str, term_y_last);
	
	while (active) {		
		draw_upper(&stdout_y, HEADER, title.str);
		draw_text(&stdout_y, &text, &scroll);
		draw_lower(cmdin, imode, &feedback);
	
		read(STDIN_FILENO, &c, 1);
		handle_key(c, &active, cmdin, &imode, &text_lines, &scroll);
	}
	
	term_restore();
	return 0;
}
