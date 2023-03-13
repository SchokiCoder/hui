/* Copyright (C) 2022 - 2023 Andy Frank Schoknecht
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

static long unsigned term_x_last, term_y_last;

struct AppMenu {
	const struct Menu *cur_menu;
	long unsigned      menu_stack_len;
	const struct Menu *menu_stack[MENU_STACK_SIZE];
	long unsigned      cursor;
};

struct AppReader {
	struct String feedback;
	long unsigned feedback_lines;
	long unsigned reader_scroll;
};

struct AppMenu AppMenu_new()
{
	struct AppMenu ret = {
		.cur_menu = &MENU_MAIN,
		.menu_stack_len = 1,
		.menu_stack[0] = &MENU_MAIN,
		.cursor = 0
	};

	return ret;
}

struct AppReader AppReader_new()
{
	struct AppReader ret = {
		.reader_scroll = 0,
		.feedback = String_new(),
		.feedback_lines = 0
	};

	return ret;
}

long unsigned count_menu_entries(const struct Menu *menu)
{
	long unsigned i;
	
	for (i = 0; menu->entries[i].type != ET_NONE; i++) {}
	
	return i;
}

/* Draw header and title.
 */
void draw_upper(long unsigned *stdout_y, const char *header, const char *title)
{
	/* draw menu text, update stdout_y */
	set_cursor(1, 1);
	hprintf(HEADER_FG, HEADER_BG, header);
	hprintf(TITLE_FG, TITLE_BG, "%s\n", title);

	*stdout_y += str_lines(header, term_x_last);
	*stdout_y += str_lines(title, term_x_last);
}

/* Jump to last line and draw the command line.
 */
void draw_lower(const char *cmdin, const struct AppReader *ardr)
{
	set_cursor(1, term_y_last);
	hprintf(CMDLINE_FG, CMDLINE_BG, CMD_PREPEND);

	if (cmdin != NULL && strlen(cmdin) > 0)
		hprintf(CMDLINE_FG, CMDLINE_BG, cmdin);
	else if (1 == ardr->feedback_lines)
		hprintf(FEEDBACK_FG, FEEDBACK_BG, ardr->feedback.str);
}

/* Draw the environment in which multi-line feedback is displayed.
 */
void
draw_reader(long unsigned          *stdout_y,
            const struct AppReader *ardr,
            const char             *cmdin)
{
	long unsigned i, text_x = 0, text_y = 0;

	/* skip the text that is scrolled over */
	for (i = 0; i < ardr->feedback.len; i += 1) {
		if (text_x >= term_x_last || '\n' == ardr->feedback.str[i]) {
			text_x = 0;
			text_y += 1;
		}

		if (text_y >= ardr->reader_scroll)
			break;

		text_x += 1;
	}

	if ('\n' == ardr->feedback.str[i])
		i += 1;

	/* print text */
	for (i = i; i < ardr->feedback.len; i += 1) {
		if (text_x >= term_x_last || '\n' == ardr->feedback.str[i]) {
			putc('\n', stdout);
			text_x = 0;
			text_y += 1;
			*stdout_y += 1;
		} else {
			putc(ardr->feedback.str[i], stdout);
			text_x += 1;
		}

		if (*stdout_y >= term_y_last)
			break;
	}
}

/* Draw the menu.
 */
void
draw_menu(long unsigned          *stdout_y,
          const struct AppMenu   *amnu,
          const char             *cmdin)
{
	long unsigned i = 0, available_rows;

	/* calc first entry to be drawn */
	available_rows = term_y_last - *stdout_y - 1;
	if (amnu->cursor > available_rows)
		i = amnu->cursor - available_rows;

	/* draw */
	for (i = i; amnu->cur_menu->entries[i].type != ET_NONE; i++) {
		if (*stdout_y >= term_y_last)
			break;

		if (amnu->cursor == i) {
			hprintf(ENTRY_HOVER_FG, ENTRY_HOVER_BG,
				"> %s\n", amnu->cur_menu->entries[i].caption);
		} else {
			hprintf(ENTRY_FG, ENTRY_BG,
				"> %s\n", amnu->cur_menu->entries[i].caption);
		}

		*stdout_y += 1;
	}
}

/* Manipulates the runtime depending on given shell string.
 */
void handle_sh(const char *sh, struct AppReader *ardr)
{
	FILE *p;
	char buf[STRING_BLOCK_SIZE];
	long unsigned buf_len;
	int read = 1;

	String_bleach(&ardr->feedback);

	p = popen(sh, "r");
	if (NULL == p) {
		String_copy(&ardr->feedback, "ERROR shell could not execute");
		ardr->feedback_lines = 1;
		return;
	}

	while (read) {
		buf_len = fread(buf, sizeof(char), STRING_BLOCK_SIZE, p);
		if (buf_len < STRING_BLOCK_SIZE)
			read = 0;

		String_append(&ardr->feedback, buf, buf_len);
	}
	pclose(p);

	String_rtrim(&ardr->feedback);
	ardr->feedback_lines = str_lines(ardr->feedback.str, term_x_last);

	if (0 == ardr->feedback_lines) {
		String_copy(&ardr->feedback, "Executed without feedback");
		ardr->feedback_lines = 1;
	}
}

/* Manipulates the runtime depending on given command.
 */
void
handle_command(const char       *cmd,
               int              *active,
               const char       *cmdin,
               struct AppMenu   *amnu,
               struct AppReader *ardr)
{
	int n;
	long unsigned menu_len;

	if (strcmp(cmd, "q") == 0
	    || strcmp(cmd, "quit") == 0
	    || strcmp(cmd, "exit") == 0) {
		*active = 0;
	} else {
		n = atoi(cmdin);

		if (n > 0) {
			menu_len = count_menu_entries(amnu->cur_menu);
			if (n >= menu_len)
				amnu->cursor = menu_len - 1;
			else
				amnu->cursor = n - 1;
		} else {
			String_copy(&ardr->feedback, "Command not recognised");
			return;
		}
	}
}

void
menu_handle_key(const char        key,
                int              *active,
                struct AppMenu   *amnu,
                struct AppReader *ardr,
                enum InputMode   *imode)
{
	switch (key) {
	case 'q':
		*active = 0;
		break;

	case 'j':
		if (amnu->cur_menu->entries[amnu->cursor + 1].type != ET_NONE)
			amnu->cursor += 1;
		break;

	case 'k':
		if (amnu->cursor > 0)
			amnu->cursor -= 1;
		break;

	case 'l':
		if (ET_SUBMENU == amnu->cur_menu->entries[amnu->cursor].type) {
			amnu->cur_menu =
			    amnu->cur_menu->entries[amnu->cursor].submenu;
			amnu->menu_stack[amnu->menu_stack_len] = amnu->cur_menu;
			amnu->menu_stack_len += 1;
			ardr->feedback_lines = 0;
			amnu->cursor = 0;
		}
		break;

	case 'h':
		if (amnu->menu_stack_len > 1) {
			amnu->menu_stack_len -= 1;
			amnu->cur_menu =
			    amnu->menu_stack[amnu->menu_stack_len - 1];
			ardr->feedback_lines = 0;
			amnu->cursor = 0;
		}
		break;

	case 'L':
		if (ET_SHELL == amnu->cur_menu->entries[amnu->cursor].type) {
			handle_sh(amnu->cur_menu->entries[amnu->cursor].shell,
				  ardr);
		}
		break;

	case ':':
		*imode = IM_CMD;
		break;

	case SIGINT:
	case SIGTSTP:
		*active = 0;
		break;
	}
}

void
reader_handle_key(const char        key,
                  int              *active,
                  struct AppReader *ardr,
                  enum InputMode   *imode)
{
	switch (key) {
	case 'q':
		*active = 0;
		break;

	case 'j':
		if (ardr->reader_scroll < (ardr->feedback_lines - 1))
			ardr->reader_scroll += 1;
		break;

	case 'k':
		if (ardr->reader_scroll > 0)
			ardr->reader_scroll -= 1;
		break;

	case SIGINT:
	case SIGTSTP:
	case 'h':
		ardr->feedback_lines = 0;
		ardr->reader_scroll = 0;
		break;

	case ':':
		*imode = IM_CMD;
		break;
	}
}

/* Manipulates the runtime depending on given key.
 */
void
handle_key(const char        key,
           int              *active,
           enum InputMode   *imode,
           char             *cmdin,
           struct AppMenu   *amnu,
           struct AppReader *ardr)
{
	if (IM_CMD == *imode) {
		switch (key) {
		case '\n':
			handle_command(cmdin, active, cmdin, amnu, ardr);

		case SIGINT:
		case SIGTSTP:
			strn_bleach(cmdin, CMD_IN_LEN);
			*imode = IM_NORMAL;
			break;

		default:
			str_add_char(cmdin, key);
			break;
		}

		return;
	}

	if (ardr->feedback_lines > 1)
		reader_handle_key(key, active, ardr, imode);
	else
		menu_handle_key(key, active, amnu, ardr, imode);
}

int main(const int argc, const char *argv[])
{
	int active = 1;
	enum InputMode imode = IM_NORMAL;
	char cmdin[CMD_IN_LEN] = "\0";
	long unsigned stdout_y;
	struct AppMenu amnu = AppMenu_new();
	struct AppReader ardr = AppReader_new();

	struct winsize wsize;
	struct termios orig, raw;
	char c;
	const long unsigned prepend_len = strlen(CMD_PREPEND);

	/* parse opts */
	if (2 == argc) {
		switch (argv[1][1]) {
		case 'v':
			printf("hui: version " VERSION "\n");
			return 0;
			break;

		case 'a':
			printf("\"House User Interface\" aka hui %s is "
			       "licensed under the BSD-3-Clause license.\n"
			       "You should have received a copy of the license "
			       "along with this program.\n\n"
			       "The source code of this program is available "
			       "at:"
			       "\nhttps://github.com/SchokiCoder/hui\n\n"
			       "Copyright 2022 - 2023 Andy Frank Schoknecht\n",
			       VERSION);
			return 0;
			break;
		}
	}

	/* get term size */
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &wsize);
	term_x_last = wsize.ws_col;
	term_y_last = wsize.ws_row;

	/* get term info and set raw mode */
	setbuf(stdout, NULL);
	tcgetattr(STDIN_FILENO, &orig);
	raw = orig;
	raw.c_lflag &= ~(ECHO | ICANON | ISIG);
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
	printf(SEQ_CRSR_HIDE);

	/* mainloop */
	while (active) {
		/* clear */
		hprintf(OVERALL_BG, OVERALL_BG, SEQ_CLEAR);
		stdout_y = 0;

		/* draw upper */
		draw_upper(&stdout_y, HEADER, amnu.cur_menu->title);

		/* draw reader or menu */
		if (ardr.feedback_lines > 1)
			draw_reader(&stdout_y, &ardr, cmdin);
		else
			draw_menu(&stdout_y, &amnu, cmdin);

		/* draw lower */
		draw_lower(cmdin, &ardr);

		if (IM_CMD == imode)
			printf(SEQ_CRSR_SHOW);
		else
			printf(SEQ_CRSR_HIDE);

		read(STDIN_FILENO, &c, 1);
		handle_key(c, &active, &imode, cmdin, &amnu, &ardr);
	}

	/* restore original terminal settings */
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig);
	printf(SEQ_CRSR_SHOW);
	printf(SEQ_FG_DEFAULT);
	printf(SEQ_BG_DEFAULT);

	String_free(&ardr.feedback);

	return 0;
}

