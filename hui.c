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
#include "menu.h"
#include "sequences.h"

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
	long unsigned i,
	              available_rows;

	/* calc first entry to be drawn */
	available_rows = term_y_last - *stdout_y - 1;
	if (cursor > available_rows)
		i = cursor - available_rows;

	/* draw */
	for (i = 0; cur_menu->entries[i].type != ET_NONE; i++) {
		if (*stdout_y >= term_y_last)
			break;

		if (cursor == i) {
			hprintf(ENTRY_HOVER_FG, ENTRY_HOVER_BG,
				"%s%s\n", ENTRY_PREPEND,
				cur_menu->entries[i].caption);
		} else {
			hprintf(ENTRY_FG, ENTRY_BG,
				"%s%s\n", ENTRY_PREPEND,
				cur_menu->entries[i].caption);
		}

		*stdout_y += 1;
	}
}

void handle_c(void (*c) (struct String *), struct String *feedback)
{
	struct String temp = String_new();
	
	String_bleach(feedback);
	
	c(&temp);
	
	String_append(feedback, temp.str, temp.len);
	
	String_free(&temp);
}

void
handle_sh(const char    *sh,
          struct String *feedback,
	  unsigned long *feedback_lines)
{
	FILE *p;
	char buf[STRING_BLOCK_SIZE];
	long unsigned buf_len;
	int read = 1;

	String_bleach(feedback);

	p = popen(sh, "r");
	if (NULL == p) {
		set_feedback(feedback,
		             feedback_lines,
			     "ERROR shell could not execute");
		return;
	}

	while (read) {
		buf_len = fread(buf, sizeof(char), STRING_BLOCK_SIZE, p);
		if (buf_len < STRING_BLOCK_SIZE)
			read = 0;

		String_append(feedback, buf, buf_len);
	}
	pclose(p);
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
	if (IM_CMD == *imode) {
		handle_key_cmdline(key,
				   cmdin,
				   active,
				   imode,
				   *cur_menu,
				   cursor,
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
		if (ET_SHELL == (*cur_menu)->entries[*cursor].type) {
			handle_sh((*cur_menu)->entries[*cursor].shell,
			          feedback,
				  feedback_lines);
		} else if (ET_C == (*cur_menu)->entries[*cursor].type) {
			handle_c((*cur_menu)->entries[*cursor].c, feedback);
		} else {
			return;
		}
		String_rtrim(feedback);
		*feedback_lines = str_lines(feedback->str, term_x_last);
		
		if (0 == feedback_lines) {
			set_feedback(feedback,
		                     feedback_lines,
				     "Executed without feedback");
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

int main(const int argc, const char **argv)
{
	int                active = 1;
	char               c;
	char               cmdin[CMD_IN_LEN] = "\0";
	long unsigned      cursor = 0;
	const struct Menu *cur_menu = &MENU_MAIN;
	enum InputMode     imode = IM_NORMAL;
	struct String      feedback = String_new();
	long unsigned      feedback_lines = 0;
	const struct Menu *menu_stack[MENU_STACK_SIZE] = {[0] = &MENU_MAIN};
	long unsigned      menu_stack_len = 1;
	long unsigned      stdout_y;

	if (handle_cmdline_opts(argc,
				argv,
				"House User Interface",
				"hui",
				VERSION) != 0)
		return 0;
	
	term_get_size();
	term_set_raw();

	while (active) {
		printf(SEQ_CLEAR);
		stdout_y = 0;

		if (feedback_lines > 1)
			system(/*PAGER + " " + */ feedback.str); // TODO

		draw_upper(&stdout_y, HEADER, cur_menu->title);
		draw_menu(&stdout_y, cursor, cur_menu);
		draw_lower(cmdin, imode, &feedback);

		if (IM_CMD == imode)
			printf(SEQ_CRSR_SHOW);
		else
			printf(SEQ_CRSR_HIDE);

		read(STDIN_FILENO, &c, 1);
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

	term_restore();

	String_free(&feedback);

	return 0;
}
