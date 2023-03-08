/* Copyright 2022 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

#include "sequences.h"
#include "hstring.h"
#include "config.h"

enum InputMode {
	IM_NORMAL = 0,
	IM_CMD =    1
};

#define VERSION "0.3.0"

#define SIGINT  '\003'
#define SIGTSTP '\032'

#define STRING_BLOCK_SIZE 1024

static long unsigned term_x_last, term_y_last;

struct Runtime {
	int                  active;
	long unsigned        cursor;
	long unsigned        reader_scroll;
	enum InputMode       imode;
	char                 cmdin[CMD_IN_LEN];
	const struct Menu   *cur_menu;
	long unsigned        menu_stack_len;
	const struct Menu   *menu_stack[MENU_STACK_SIZE];
	char                *feedback;
	long unsigned        feedback_len;
	long unsigned        feedback_size;
	long unsigned        feedback_lines;
};

void init_runtime(struct Runtime *rt)
{
	rt->active = -1;
	rt->imode = IM_NORMAL;
	rt->cmdin[0] = '\0';
	rt->cursor = 0;
	rt->reader_scroll = 0;
	rt->feedback = malloc(STRING_BLOCK_SIZE);
	rt->feedback_len = 0;
	rt->feedback_size = STRING_BLOCK_SIZE;
	rt->feedback_lines = 0;	
	rt->menu_stack_len = 1;
	rt->menu_stack[0] = &MENU_MAIN;
	rt->cur_menu = &MENU_MAIN;
}

long unsigned count_menu_entries(const struct Menu *menu)
{
	long unsigned i;
	
	for (i = 0; menu->entries[i].type != ET_NONE; i++) {}
	
	return i;
}

/* Returns the amount of lines needed for this print.
 */
long unsigned draw_upper(const char *header, const char *title)
{
	long unsigned ret = 0;
	
	/* draw bg color (clear) */
	hprintf(OVERALL_BG, OVERALL_BG, SEQ_CLEAR);
	
	/* draw menu text */
	set_cursor(1, 1);
	hprintf(HEADER_FG, HEADER_BG, header);
	hprintf(TITLE_FG, TITLE_BG, "%s\n", title);

	ret += str_lines(header, term_x_last);
	ret += str_lines(title, term_x_last);
	return ret;
}

/* Draw the lower part.
 * Doesn't manipulate the runtime.
 */
void draw_lower(const struct Runtime *rt)
{
	set_cursor(1, term_y_last);
	hprintf(CMDLINE_FG, CMDLINE_BG, CMD_PREPEND);

	if (rt->cmdin != NULL && strlen(rt->cmdin) > 0)
		hprintf(CMDLINE_FG, CMDLINE_BG, rt->cmdin);
	else if (1 == rt->feedback_lines)
		hprintf(FEEDBACK_FG, FEEDBACK_BG, rt->feedback);
}

/* Draw the environment in which multi-line feedback is displayed.
 * Doesn't manipulate the runtime.
 */
void draw_reader(const char *header, const struct Runtime *rt)
{
	long unsigned i, text_x = 0, text_y = 0, stdout_y;
	
	stdout_y = draw_upper(header, rt->cur_menu->title);
	
	/* skip the text that is scrolled over */
	for (i = 0; i < rt->feedback_len; i += 1) {
		if (text_x >= term_x_last || rt->feedback[i] == '\n') {
			text_x = 0;
			text_y += 1;
		}
		
		if (text_y >= rt->reader_scroll)
			break;
		
		text_x += 1;
	}
	
	if ('\n' == rt->feedback[i])
		i += 1;

	/* print text */
	for (i = i; i < rt->feedback_len; i += 1) {
		if (text_x >= term_x_last || rt->feedback[i] == '\n') {
			putc('\n', stdout);
			text_x = 0;
			text_y += 1;
			stdout_y += 1;
		} else {
			putc(rt->feedback[i], stdout);
			text_x += 1;
		}
		
		if (stdout_y >= term_y_last)
			break;
	}

	draw_lower(rt);
}

/* Doesn't manipulate the runtime.
 */
void draw_menu(const char *header, const struct Runtime *rt)
{
	long unsigned i = 0, available_rows, stdout_y;
	stdout_y = draw_upper(header, rt->cur_menu->title);
	
	/* calc first entry to be drawn */
	available_rows = term_y_last - stdout_y - 1;
	if (rt->cursor > available_rows)
		i = rt->cursor - available_rows;

	/* draw */
	for (i = i; rt->cur_menu->entries[i].type != ET_NONE; i++) {
		if (stdout_y >= term_y_last)
			break;
			
		if (i == rt->cursor) {
			hprintf(ENTRY_HOVER_FG, ENTRY_HOVER_BG,
			        "> %s\n", rt->cur_menu->entries[i].caption);
		}
		else {
			hprintf(ENTRY_FG, ENTRY_BG,
			        "> %s\n", rt->cur_menu->entries[i].caption);
		}
		
		stdout_y += 1;
	}
	
	draw_lower(rt);
}

/* Manipulates the runtime depending on given shell string.
 */
void handle_sh(const char *sh, struct Runtime *rt)
{
	FILE *p;
	char buf[STRING_BLOCK_SIZE];
	long unsigned buf_len;
	int read = 1;
	
	strn_bleach(rt->feedback, rt->feedback_len);
	rt->feedback_len = 0;
	rt->feedback[0] = '\0';
	
	p = popen(sh, "r");
	if (NULL == p) {
		strcpy(rt->feedback, "ERROR shell could not execute");
		rt->feedback_lines = 1;
		return;
	}
	
	while (read) {
		buf_len = fread(buf, sizeof(char), STRING_BLOCK_SIZE, p);
		if (buf_len < STRING_BLOCK_SIZE)
			read = 0;
		
		if (buf_len > (rt->feedback_size - rt->feedback_len)) {
			rt->feedback_size += STRING_BLOCK_SIZE;
			rt->feedback = realloc(rt->feedback, rt->feedback_size);
		}
		
		strncpy(&rt->feedback[rt->feedback_len], buf, buf_len);
		rt->feedback_len += buf_len;
	}
	pclose(p);
	
	str_rtrim(rt->feedback);
	rt->feedback_lines = str_lines(rt->feedback, term_x_last);
	
	if (rt->feedback_lines == 0) {
		strcpy(rt->feedback, "Executed without feedback");
		rt->feedback_lines = 1;
	}
}

/* Manipulates the runtime depending on given command.
 */
void handle_command(const char *cmd, struct Runtime *rt)
{
	int n;
	long unsigned menu_len;
	
	if (strcmp(cmd, "q") == 0
	    || strcmp(cmd, "quit") == 0
	    || strcmp(cmd, "exit") == 0) {
		rt->active = 0;
	} else {
		n = atoi(rt->cmdin);
		
		if (n > 0) {
			menu_len = count_menu_entries(rt->cur_menu);
			if (n >= menu_len)
				rt->cursor = menu_len - 1;
			else
				rt->cursor = n - 1;
		} else {
			rt->feedback = "Command not recognised";
			return;
		}
	}
}

void menu_handle_key(const char key, struct Runtime *rt)
{
	switch (key) {
	case 'q':
		rt->active = 0;
		break;

	case 'j':
		if (rt->cur_menu->entries[rt->cursor + 1].type != ET_NONE)
			rt->cursor += 1;
		break;

	case 'k':
		if (rt->cursor > 0)
			rt->cursor -= 1;
		break;

	case 'l':
		if (rt->cur_menu->entries[rt->cursor].type == ET_SUBMENU) {
			rt->cur_menu = rt->cur_menu->entries[rt->cursor].submenu;
			rt->menu_stack[rt->menu_stack_len] = rt->cur_menu;
			rt->menu_stack_len += 1;
			rt->feedback_lines = 0;
			rt->cursor = 0;
		}
		break;

	case 'h':
		if (rt->menu_stack_len > 1) {
			rt->menu_stack_len -= 1;
			rt->cur_menu = rt->menu_stack[rt->menu_stack_len - 1];
			rt->feedback_lines = 0;
			rt->cursor = 0;
		}
		break;
	
	case 'L':
		if (rt->cur_menu->entries[rt->cursor].type == ET_SHELL) {
			handle_sh(rt->cur_menu->entries[rt->cursor].shell, rt);
		}
		break;

	case ':':
		rt->imode = IM_CMD;
		break;

	case SIGINT:
	case SIGTSTP:
		rt->active = 0;
		break;
	}
}

void reader_handle_key(const char key, struct Runtime *rt)
{
	switch (key) {
	case 'q':
		rt->active = 0;
		break;

	case 'j':
		if (rt->reader_scroll < (rt->feedback_lines - 1))
			rt->reader_scroll += 1;
		break;

	case 'k':
		if (rt->reader_scroll > 0)
			rt->reader_scroll -= 1;
		break;
	
	case 'h':
		rt->feedback_lines = 0;
		rt->reader_scroll = 0;
		break;

	case ':':
		rt->imode = IM_CMD;
		break;

	case SIGINT:
	case SIGTSTP:
		rt->active = 0;
		break;
	}
}

/* Manipulates the runtime depending on given key.
 * Returns:
 * 0 - nothing needs to be done by calling code
 * 1 - the mainloop should "continue"
 */
void handle_key(const char key, struct Runtime *rt)
{
	if (rt->imode == IM_CMD) {
		switch (key) {
		case '\n':
			handle_command(rt->cmdin, rt);
			
		case SIGINT:
		case SIGTSTP:
			strn_bleach(rt->cmdin, CMD_IN_LEN);
			rt->imode = IM_NORMAL;
			break;

		default:
			str_add_char(rt->cmdin, key);
			break;
		}

		return;
	}

	if (rt->feedback_lines > 1)
		reader_handle_key(key, rt);
	else
		menu_handle_key(key, rt);
}

int main(int argc, char *argv[])
{
	struct winsize wsize;
	struct termios orig, raw;
	char c;
	struct Runtime rt;
	const long unsigned prepend_len = strlen(CMD_PREPEND);
	
	/* parse opts */
	if (argc == 2 && !strcmp("-v", argv[1])) {
		printf("hui: version " VERSION "\n");
		return 0;
	}
	
	/* get term size */
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &wsize);
	term_x_last = wsize.ws_col;
	term_y_last = wsize.ws_row;
	
	init_runtime(&rt);

	/* get term info and set raw mode */
	setbuf(stdout, NULL);
	tcgetattr(STDIN_FILENO, &orig);
	raw = orig;
	raw.c_lflag &= ~(ECHO | ICANON | ISIG);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);	
	printf(SEQ_CRSR_HIDE);

	/* mainloop */
	while (rt.active) {
		if (rt.feedback_lines > 1)
			draw_reader(HEADER, &rt);
		else
			draw_menu(HEADER, &rt);

		if (rt.imode == IM_CMD)
			printf(SEQ_CRSR_SHOW);
		else
			printf(SEQ_CRSR_HIDE);

		read(STDIN_FILENO, &c, 1);
		handle_key(c, &rt);
	}

	/* restore original terminal settings */
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig);
	printf(SEQ_CRSR_SHOW);
	printf(SEQ_FG_DEFAULT);
	printf(SEQ_BG_DEFAULT);
	
	free(rt.feedback);

	return 0;
}

