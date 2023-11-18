/* Copyright (C) 2022 - 2023 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

/* This is the hui main file.
 */

#define _SCRIPTS_H_IMPL

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "color.h"
#include "common.h"
#include "config.h"
#include "hstring.h"
#include "license_str.h"
#include "menu.h"
#include "sequences.h"

long unsigned term_x_len,
	      term_y_len;

long unsigned count_menu_entries(const struct Menu *menu);

void
draw_menu(long unsigned       *stdout_y,
          const long unsigned  cursor,
	  const struct Menu   *cur_menu);

void handle_c(void (*c) (struct String *), struct String *feedback);

void
handle_cmd(const char          *cmdin,
	   int                 *active,
	   const struct Menu   *cur_menu,
	   long unsigned       *cursor,
	   struct String       *feedback,
	   unsigned long       *feedback_lines);

int handle_cmdline_opts(const int argc, const char **argv);

void
handle_key(const char          key,
           int                *active,
	   enum InputMode     *imode,
	   char               *cmdin,
           const struct Menu **cur_menu,
	   long unsigned      *cursor,
	   long unsigned      *menu_stack_len,
	   const struct Menu **menu_stack,
           struct String      *feedback,
	   long unsigned      *feedback_lines);

void
handle_key_cmdline(const char           key,
		   char                *cmdin,
		   int                 *active,
		   long unsigned       *cursor,
		   const struct Menu   *cur_menu,
		   enum InputMode      *imode,
		   struct String       *feedback,
		   unsigned long       *feedback_lines);

void
handle_sh(const char    *sh,
          struct String *feedback,
	  unsigned long *feedback_lines,
	  const int      child_has_mainloop);

long unsigned count_menu_entries(const struct Menu *menu)
{
	long unsigned i;
	
	for (i = 0; menu->entries[i].type != ET_NONE; i++) {}
	
	return i;
}

void
draw_menu(long unsigned       *stdout_y,
          const long unsigned  cursor,
	  const struct Menu   *cur_menu)
{
	long unsigned i = 0,
	              available_rows;
	const char *entry_prefix,
	           *entry_postfix;

	/* calc first entry to be drawn */
	available_rows = term_y_len - *stdout_y - 1;
	if (cursor > available_rows)
		i = cursor - available_rows;

	/* draw */
	for (;; i++) {
		if (*stdout_y >= term_y_len)
			break;

		switch (cur_menu->entries[i].type) {
		case ET_NONE:
			return;
			break;

		case ET_C:
			entry_prefix = ET_C_PREFIX;
			entry_postfix = ET_C_POSTFIX;
			break;

		case ET_SHELL:
			entry_prefix = ET_SHELL_PREFIX;
			entry_postfix = ET_SHELL_POSTFIX;
			break;

		case ET_SHELL_LONG:
			entry_prefix = ET_SHELL_LONG_PREFIX;
			entry_postfix = ET_SHELL_LONG_POSTFIX;
			break;

		case ET_SUBMENU:
			entry_prefix = ET_SUBMENU_PREFIX;
			entry_postfix = ET_SUBMENU_POSTFIX;
			break;
		}

		if (cursor == i) {
			hprintf(ENTRY_HOVER_FG, ENTRY_HOVER_BG,
				"%s%s%s\n", entry_prefix,
				cur_menu->entries[i].caption,
				entry_postfix);
		} else {
			hprintf(ENTRY_FG, ENTRY_BG,
				"%s%s%s\n", entry_prefix,
				cur_menu->entries[i].caption,
				entry_postfix);
		}

		*stdout_y += 1;
	}
}

void handle_c(void (*c) (struct String *), struct String *feedback)
{
	struct String temp = String_new();
	
	c(&temp);
	String_copy(feedback, temp.str, temp.size);
	String_free(&temp);
}

void
handle_cmd(const char          *cmdin,
	   int                 *active,
	   const struct Menu   *cur_menu,
	   long unsigned       *cursor,
	   struct String       *feedback,
	   unsigned long       *feedback_lines)
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
			if ((long unsigned) n >= menu_len)
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

int handle_cmdline_opts(const int argc, const char **argv)
{
	if (2 == argc) {
		switch (argv[1][1]) {
		case 'v':
			printf("%s: version %s\n", "hui", VERSION);
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
			       "House User Interface", "hui", VERSION,
			       MSG_COPYRIGHT, MSG_CLAUSES, MSG_WARRANTY);
			return 1;
			break;
		
		default:
			return 0;
		}
	}
	
	return 0;
}

void
handle_key_cmdline(const char           key,
		   char                *cmdin,
		   int                 *active,
		   long unsigned       *cursor,
		   const struct Menu   *cur_menu,
		   enum InputMode      *imode,
		   struct String       *feedback,
		   unsigned long       *feedback_lines)
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
			strn_bleach(cmdin, CMD_IN_SIZE);
			*imode = IM_NORMAL;
			break;

		default:
			strn_add_char(cmdin, key, strlen(cmdin), CMD_IN_SIZE);
			break;
	}
}

void
handle_key(const char          key,
           int                *active,
	   enum InputMode     *imode,
	   char               *cmdin,
           const struct Menu **cur_menu,
	   long unsigned      *cursor,
	   long unsigned      *menu_stack_len,
	   const struct Menu **menu_stack,
           struct String      *feedback,
	   long unsigned      *feedback_lines)
{
	int sh_long = 0;
	
	if (IM_CMD == *imode) {
		handle_key_cmdline(key,
				   cmdin,
				   active,
				   cursor,
				   *cur_menu,
				   imode,
				   feedback,
				   feedback_lines);
		return;
	}
	
	switch (key) {
	case KEY_QUIT:
		*active = 0;
		break;

	case KEY_DOWN:
		if ((*cur_menu)->entries[*cursor + 1].type != ET_NONE)
			*cursor += 1;
		break;

	case KEY_UP:
		if (*cursor > 0)
			*cursor -= 1;
		break;

	case KEY_RIGHT:
		if (ET_SUBMENU == (*cur_menu)->entries[*cursor].type) {
			*cur_menu = (*cur_menu)->entries[*cursor].submenu;
			menu_stack[*menu_stack_len] = *cur_menu;
			*menu_stack_len += 1;
			feedback_lines = 0;
			*cursor = 0;
		}
		break;

	case KEY_LEFT:
		if (*menu_stack_len > 1) {
			*menu_stack_len -= 1;
			*cur_menu = menu_stack[*menu_stack_len - 1];
			feedback_lines = 0;
			*cursor = 0;
		}
		break;

	case KEY_EXEC:
		switch ((*cur_menu)->entries[*cursor].type) {
		case ET_SHELL_LONG:
			sh_long = 1;
			/* fallthrough */
		case ET_SHELL:
			handle_sh((*cur_menu)->entries[*cursor].shell,
			          feedback,
				  feedback_lines,
				  sh_long);
			break;
		
		case ET_C:
			handle_c((*cur_menu)->entries[*cursor].c, feedback);
			break;
		
		default:
			return;
			break;
		}
		
		String_rtrim(feedback);
		*feedback_lines = strn_lines(feedback->str,
					     feedback->size,
					     term_x_len);
		
		if (0 == feedback_lines) {
			set_feedback(feedback,
		                     feedback_lines,
				     "Executed without feedback",
				     term_y_len);
		}
		break;

	case KEY_CMD:
		*imode = IM_CMD;
		break;

	case SIGINT:
	case SIGTSTP:
		*active = 0;
		break;
	}
}

void
handle_sh(const char    *sh,
          struct String *feedback,
	  unsigned long *feedback_lines,
	  const int      child_has_mainloop)
{
	FILE *p;

	if (child_has_mainloop) {
		printf(SEQ_CLEAR);
		term_restore();
		system(sh);
		term_set_raw();
	} else {
		String_bleach(feedback);

		p = popen(sh, "r");
		if (NULL == p) {
			set_feedback(feedback,
				     feedback_lines,
				     "ERROR shell could not execute",
				     term_y_len);
			return;
		}

		String_read_file(feedback, p);
		pclose(p);
	}
}

int main(const int argc, const char **argv)
{
	int                  active = 1;
	char                 c;
	char                 cmdin[CMD_IN_SIZE] = "\0";
	long unsigned        cursor = 0;
	const struct Menu   *cur_menu = &MENU_MAIN;
	const long unsigned  header_size = strlen(HEADER);
	enum InputMode       imode = IM_NORMAL;
	struct String        feedback = String_new();
	long unsigned        feedback_lines = 0;
	const struct Menu   *menu_stack[MENU_STACK_SIZE] = {[0] = &MENU_MAIN};
	long unsigned        menu_stack_len = 1;
	long unsigned        stdout_y;

	if (handle_cmdline_opts(argc, argv) != 0)
		return 0;
	
	term_get_size(&term_x_len, &term_y_len);
	term_set_raw();

	while (active) {
		stdout_y = 0;

		if (feedback_lines > 1)
			call_pager(&feedback, &feedback_lines, term_y_len);

		printf(SEQ_CLEAR);

		draw_upper(HEADER,
			   header_size,
			   &stdout_y,
			   cur_menu->title,
			   strlen(cur_menu->title),
			   term_x_len);
		draw_menu(&stdout_y, cursor, cur_menu);
		draw_lower(cmdin, &feedback, feedback_lines, imode, term_y_len);

		if (IM_CMD == imode)
			printf(SEQ_CRSR_SHOW);
		else
			printf(SEQ_CRSR_HIDE);

		if (read(STDIN_FILENO, &c, 1) > 0) {
			handle_key(c,
				   &active,
				   &imode,
				   cmdin,
				   &cur_menu,
				   &cursor,
				   &menu_stack_len,
				   menu_stack,
				   &feedback,
				   &feedback_lines);
		}
	}

	term_restore();

#ifndef STRING_NOT_ON_HEAP
	String_free(&feedback);
#endif

	return 0;
}
