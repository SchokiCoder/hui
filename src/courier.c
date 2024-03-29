/* Copyright (C) 2022 - 2024 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

/* This is the main file of the hui pager, called "courier".
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "config.h"
#include "hstring.h"
#include "license_header.h"
#include "sequences.h"

long unsigned term_x_len,
	      term_y_len;

void
draw_content(const struct String *content,
	     const long unsigned *scroll,
	     long unsigned       *stdout_y);

void
handle_cmd(const char          *cmdin,
	   int                 *active,
	   const long unsigned  content_lines,
	   struct String       *feedback,
	   unsigned long       *feedback_lines,
	   long unsigned       *scroll);

int
handle_cmdline_opts(const int argc, const char **argv, struct String *title);

void
handle_key(const char         key,
           int               *active,
           char              *cmdin,
	   long unsigned     *content_lines,
           struct String     *feedback,
	   unsigned long     *feedback_lines,
	   enum InputMode    *imode,
           long unsigned     *scroll);

void
handle_key_cmdline(const char           key,
		   char                *cmdin,
		   int                 *active,
		   const long unsigned  content_lines,
		   struct String       *feedback,
		   unsigned long       *feedback_lines,
		   enum InputMode      *imode,
		   long unsigned       *scroll);

/* First argument from argv not starting with a dash is used as path,
 * except when a "-t" is given before that.
 */
FILE* open_target_file(const int argc, const char **argv);

void read_target_file(FILE *file, struct String *content);

void
draw_content(const struct String *content,
	     const long unsigned *scroll,
	     long unsigned       *stdout_y)
{
	long unsigned i,
	              content_x = 0,
		      content_y = 0;

	/* skip the content that is scrolled over */
	for (i = 0; i < content->len; i += 1) {
		if (content_x >= term_x_len)
			i -= 1;

		if (content_x >= term_x_len || '\n' == content->str[i]) {
			content_x = 0;
			content_y += 1;
		} else {
			content_x += 1;
		}

		if (content_y >= *scroll)
			break;
	}

	if (*scroll > 0)
		i += 1;

	/* print content */
	set_fg(CONTENT_FG);
	set_bg(CONTENT_BG);
	
	for (; i < content->len; i += 1) {
		if (content_x >= term_x_len)
			i -= 1;

		if (content_x >= term_x_len || '\n' == content->str[i]) {
			fputc('\n', stdout);
			content_x = 0;
			content_y += 1;
			*stdout_y += 1;
		} else {
			fputc(content->str[i], stdout);
			content_x += 1;
		}

		if (*stdout_y >= term_y_len)
			break;
	}
}

void
handle_cmd(const char          *cmdin,
	   int                 *active,
	   const long unsigned  content_lines,
	   struct String       *feedback,
	   unsigned long       *feedback_lines,
	   long unsigned       *scroll)
{
	long long n;

	if (strcmp(cmdin, "q") == 0
	    || strcmp(cmdin, "quit") == 0
	    || strcmp(cmdin, "exit") == 0) {
		*active = 0;
		return;
	}

	n = atoll(cmdin);

	if (n > 0) {
		if ((long unsigned) n >= content_lines) {
			*scroll = content_lines - 1;
		} else {
			*scroll = n - 1;
		}
		return;
	}
	
	set_feedback(feedback,
		     feedback_lines,
		     "Command not recognised",
		     term_y_len);
}

int
handle_cmdline_opts(const int argc, const char **argv, struct String *title)
{
	if (argc >= 2) {
		switch (argv[1][1]) {
		case 'v':
			printf("%s: version %s\n", "courier", VERSION);
			return 1;
			break;

		case 'a':
			printf("The source code of \"%s\" aka %s %s is available"
			       " at:"
			       "\nhttps://github.com/SchokiCoder/hui\n\n"
			       "If you did not receive a copy of the license, "
			       "see below:\n\n"
			       "%s\n",
			       "Courier", "courier", VERSION,
			       MSG_LICENSE);
			return 1;
			break;
		
		case 't':
			if (argc <= 2) {
				fprintf(stderr, "Argument missing\n");
				return 1;
			}
			
			String_copy(title, argv[2], strlen(argv[2]));
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
	   long unsigned     *content_lines,
           struct String     *feedback,
	   unsigned long     *feedback_lines,
	   enum InputMode    *imode,
           long unsigned     *scroll)
{
	if (IM_CMD == *imode) {		   
		handle_key_cmdline(key,
				   cmdin,
				   active,
				   *content_lines,
				   feedback,
				   feedback_lines,
				   imode,
				   scroll);
		return;
	}
	
	switch (key) {
	case KEY_DOWN:
		if (*scroll < (*content_lines - 1))
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
		*active = 0;
		break;

	case KEY_CMD:
		*imode = IM_CMD;
		break;
	}
}

void
handle_key_cmdline(const char           key,
		   char                *cmdin,
		   int                 *active,
		   const long unsigned  content_lines,
		   struct String       *feedback,
		   unsigned long       *feedback_lines,
		   enum InputMode      *imode,
		   long unsigned       *scroll)
{
	switch (key) {
		case '\n':
			handle_cmd(cmdin,
			           active,
				   content_lines,
				   feedback,
				   feedback_lines,
				   scroll);
			/* fall through */
		case SIGINT:
		case SIGTSTP:
			strn_bleach(cmdin, CMD_IN_SIZE);
			*imode = IM_NORMAL;
			break;

		default:
			strn_add_char(cmdin, key, strlen(cmdin), CMD_IN_SIZE);
			break;
	}
}

FILE* open_target_file(const int argc, const char **argv)
{
	int i;
	int explicit = 0;
	
	for (i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
			explicit = 1;
			break;
		} else if (argv[i][1] == 't') {
			i++;
		}
	}
	
	if (explicit) {
		return fopen(argv[i], "r");
	}
	
	return stdin;
}

int main(const int argc, const char **argv)
{
	int                 active = 1;
	char                c;
	char                cmdin[CMD_IN_SIZE] = "\0";
	struct String       content = String_new();
	long unsigned       content_lines = 0;
	struct String       feedback = String_new();
	long unsigned       feedback_lines = 0;
	const long unsigned header_size = strlen(HEADER);
	enum InputMode      imode = IM_NORMAL;
	long unsigned       scroll = 0;
	long unsigned       stdout_y;
	FILE               *target_file;
	struct String       title = String_new();

	if (handle_cmdline_opts(argc, argv, &title) != 0)
		goto cleanup;
	
	target_file = open_target_file(argc, argv);
	if (NULL == target_file) {
		fprintf(stderr, "File could not be opened.\n");
		goto cleanup;
	}

	String_read_file(&content, target_file);

	if (stdin == target_file)
		freopen("/dev/tty", "rw", stdin);
	else
		fclose(target_file);

	term_get_size(&term_x_len, &term_y_len);
	term_set_raw();
	
	content_lines = strn_lines(content.str, content.size, term_x_len);
	
	while (active) {
		stdout_y = 0;

		if (feedback_lines > 1)
			call_pager(&feedback, &feedback_lines, term_y_len);

		printf(SEQ_CLEAR);
		
		draw_upper(HEADER,
			   header_size,
			   &stdout_y,
			   title.str,
			   title.len,
			   term_y_len);
		draw_content(&content, &scroll, &stdout_y);
		draw_lower(cmdin, &feedback, feedback_lines, imode, term_y_len);

		if (read(STDIN_FILENO, &c, 1) > 0) {
			handle_key(c,
				   &active,
				   cmdin,
				   &content_lines,
				   &feedback,
				   &feedback_lines,
				   &imode,
				   &scroll);
		}
	}

cleanup:
#ifndef STRING_NOT_ON_HEAP
	String_free(&feedback);
	String_free(&content);
	String_free(&title);
#endif
	
	term_restore();
	return 0;
}
