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

#include "license_str.h"
#include "sequences.h"
#include "hstring.h"
#include "config.h"

enum InputMode {
	IM_NORMAL = 0,
	IM_CMD =    1
};

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
	struct String text;
	long unsigned text_lines;
	long unsigned scroll;
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
		.scroll = 0,
		.text = String_new(),
		.text_lines = 0
	};

	return ret;
}

void AppReader_set_feedback(struct AppReader *ardr, const char *str)
{
	String_copy(&ardr->text, str);
	ardr->text_lines = str_lines(str, term_y_last);
}

/* Usually called after a script ran (shell or c), to update the text state.
 */
void AppReader_handle_text(struct AppReader *ardr)
{
	String_rtrim(&ardr->text);
	ardr->text_lines = str_lines(ardr->text.str, term_x_last);
	
	if (0 == ardr->text_lines)
		AppReader_set_feedback(ardr, "Executed without feedback");
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
void
draw_lower(const char             *cmdin,
           const enum InputMode    imode,
           const struct AppReader *ardr)
{
	set_cursor(1, term_y_last);
	hprintf(CMDLINE_FG, CMDLINE_BG, CMD_PREPEND);

	if (IM_CMD == imode)
		hprintf(CMDLINE_FG, CMDLINE_BG, cmdin);
	else if (1 == ardr->text_lines)
		hprintf(FEEDBACK_FG, FEEDBACK_BG, ardr->text.str);
}

/* Draw the environment in which multi-line feedback is displayed.
 */
void draw_reader(long unsigned *stdout_y, const struct AppReader *ardr)
{
	long unsigned i, text_x = 0, text_y = 0;

	/* skip the text that is scrolled over */
	for (i = 0; i < ardr->text.len; i += 1) {
		if (text_x >= term_x_last)
			i -= 1;

		if (text_x >= term_x_last || '\n' == ardr->text.str[i]) {
			text_x = 0;
			text_y += 1;
		} else {
			text_x += 1;
		}

		if (text_y >= ardr->scroll)
			break;
	}

	if (ardr->scroll > 0)
		i += 1;

	/* print text */
	set_fg(READER_FG);
	set_bg(READER_BG);
	
	for (i = i; i < ardr->text.len; i += 1) {
		if (text_x >= term_x_last)
			i -= 1;

		if (text_x >= term_x_last || '\n' == ardr->text.str[i]) {
			fputc('\n', stdout);
			text_x = 0;
			text_y += 1;
			*stdout_y += 1;
		} else {
			fputc(ardr->text.str[i], stdout);
			text_x += 1;
		}

		if (*stdout_y >= term_y_last)
			break;
	}
}

/* Draw the menu.
 */
void draw_menu(long unsigned *stdout_y, const struct AppMenu *amnu)
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
				"%s%s\n", ENTRY_PREPEND,
				amnu->cur_menu->entries[i].caption);
		} else {
			hprintf(ENTRY_FG, ENTRY_BG,
				"%s%s\n", ENTRY_PREPEND,
				amnu->cur_menu->entries[i].caption);
		}

		*stdout_y += 1;
	}
}

/* Manipulates the runtime depending on given c function pointer.
 */
void handle_c(void (*c) (struct String *), struct AppReader *ardr)
{
	struct String feedback = String_new();
	
	String_bleach(&ardr->text);
	
	c(&feedback);
	
	String_append(&ardr->text, feedback.str, feedback.len);
	
	String_free(&feedback);
}

/* Manipulates the runtime depending on given shell string.
 */
void handle_sh(const char *sh, struct AppReader *ardr)
{
	FILE *p;
	char buf[STRING_BLOCK_SIZE];
	long unsigned buf_len;
	int read = 1;

	String_bleach(&ardr->text);

	p = popen(sh, "r");
	if (NULL == p) {
		AppReader_set_feedback(ardr, "ERROR shell could not execute");
		return;
	}

	while (read) {
		buf_len = fread(buf, sizeof(char), STRING_BLOCK_SIZE, p);
		if (buf_len < STRING_BLOCK_SIZE)
			read = 0;

		String_append(&ardr->text, buf, buf_len);
	}
	pclose(p);
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
	long long n;
	long unsigned menu_len;

	if (strcmp(cmd, "q") == 0
	    || strcmp(cmd, "quit") == 0
	    || strcmp(cmd, "exit") == 0) {
		*active = 0;
	} else {
		n = atoll(cmdin);

		if (n > 0) {
			menu_len = count_menu_entries(amnu->cur_menu);
			if ((unsigned long) n >= menu_len)
				amnu->cursor = menu_len - 1;
			else
				amnu->cursor = n - 1;
		} else {
			AppReader_set_feedback(ardr, "Command not recognised");
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
	case KEY_QUIT:
		*active = 0;
		break;

	case KEY_DOWN:
		if (amnu->cur_menu->entries[amnu->cursor + 1].type != ET_NONE)
			amnu->cursor += 1;
		break;

	case KEY_UP:
		if (amnu->cursor > 0)
			amnu->cursor -= 1;
		break;

	case KEY_RIGHT:
		if (ET_SUBMENU == amnu->cur_menu->entries[amnu->cursor].type) {
			amnu->cur_menu =
			    amnu->cur_menu->entries[amnu->cursor].submenu;
			amnu->menu_stack[amnu->menu_stack_len] = amnu->cur_menu;
			amnu->menu_stack_len += 1;
			ardr->text_lines = 0;
			amnu->cursor = 0;
		}
		break;

	case KEY_LEFT:
		if (amnu->menu_stack_len > 1) {
			amnu->menu_stack_len -= 1;
			amnu->cur_menu = 
				amnu->menu_stack[amnu->menu_stack_len - 1];
			ardr->text_lines = 0;
			amnu->cursor = 0;
		}
		break;

	case KEY_EXEC:
		if (ET_SHELL == amnu->cur_menu->entries[amnu->cursor].type) {
			handle_sh(amnu->cur_menu->entries[amnu->cursor].shell,
				  ardr);
		} else if (ET_C == amnu->cur_menu->entries[amnu->cursor].type) {
			handle_c(amnu->cur_menu->entries[amnu->cursor].c, ardr);
		} else {
			return;
		}
		AppReader_handle_text(ardr);
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
reader_handle_key(const char        key,
                  int              *active,
                  struct AppReader *ardr,
                  enum InputMode   *imode)
{
	switch (key) {
	case KEY_QUIT:
		*active = 0;
		break;

	case KEY_DOWN:
		if (ardr->scroll < (ardr->text_lines - 1))
			ardr->scroll += 1;
		break;

	case KEY_UP:
		if (ardr->scroll > 0)
			ardr->scroll -= 1;
		break;

	case SIGINT:
	case SIGTSTP:
	case KEY_LEFT:
		ardr->text_lines = 0;
		ardr->scroll = 0;
		break;

	case KEY_CMD:
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

		return;
	}

	if (ardr->text_lines > 1)
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
			       "If you did not receive a copy of the license, "
			       "see below:\n\n"
			       "%s\n\n%s\n\n%s\n",
			       VERSION,
			       MSG_COPYRIGHT, MSG_CLAUSES, MSG_WARRANTY);
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
		printf(SEQ_CLEAR);
		stdout_y = 0;

		/* draw upper */
		draw_upper(&stdout_y, HEADER, amnu.cur_menu->title);

		/* draw reader or menu */
		if (ardr.text_lines > 1)
			draw_reader(&stdout_y, &ardr);
		else
			draw_menu(&stdout_y, &amnu);

		/* draw lower */
		draw_lower(cmdin, imode, &ardr);

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

	String_free(&ardr.text);

	return 0;
}

