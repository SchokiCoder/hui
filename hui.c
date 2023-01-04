/* Copyright 2022 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#include <stdio.h>
#include <stdint.h>
#include <termbox.h>

#include "config.h"

struct TermCursor {
	long unsigned x, y;
};

/* print function, supporting colors,
 * which counts the amount of new line characters
 * and line breaks due to terminal width
 */
void hprint(struct TermCursor *tc, const char *str, uint16_t fg, uint16_t bg)
{
	long unsigned i;

	for (i = 0; str[i] != '\0'; i++) {
		tb_change_cell(tc->x, tc->y, str[i], fg, bg);
		tc->x++;

		if (tc->x > tb_width() || str[i] == '\n') {
			tc->x = 0;
			tc->y++;
		}
	}
}

void
draw_menu(struct TermCursor *tc, const char *header, const struct Menu menu,
	  const long unsigned cursor)
{
	long unsigned i;
	uint16_t fg, bg;

	hprint(tc, header, TB_WHITE, TB_DEFAULT);
	hprint(tc, menu.title, TB_WHITE, TB_DEFAULT);
	hprint(tc, "\n", TB_WHITE, TB_DEFAULT);

	/* TODO this only works as long as the menu struct is cleanly allocated
	 * find out if this could hinder portability
	 */
	for (i = 0; menu.entries[i].type != ET_NONE; i++) {
		if (i == cursor) {
			fg = TB_DEFAULT;
			bg = TB_WHITE;
		}
		else {
			fg = TB_WHITE;
			bg = TB_DEFAULT;
		}
		
		hprint(tc, "> ", fg, bg);
		hprint(tc, MENU_MAIN.entries[i].caption, fg, bg);
		hprint(tc, "\n", fg, bg);
	}

	for (i = 0; tc->y < (tb_height() - 1); i++)
		hprint(tc, "\n", TB_WHITE, TB_DEFAULT);

	hprint(tc, ":", TB_WHITE, TB_DEFAULT);
}

int main()
{
	int active = -1;
	long unsigned cursor = 0;
	struct tb_event event;
	struct TermCursor tc;

	if (tb_init() < 0) {
		printf("Termbox init failed");
		goto cleanup;
	}
	
	tb_select_output_mode(TB_OUTPUT_NORMAL);
	tb_set_clear_attributes(TB_WHITE, TB_DEFAULT);

	while (active) {
		tc.x = 0;
		tc.y = 0;
		
		tb_clear();
		
		draw_menu(&tc, HEADER, MENU_MAIN, cursor);
		
		tb_present();

		/* key handling */
		if (tb_poll_event(&event) == -1) {
			hprint(&tc, "Poll event failed", TB_RED, TB_BLACK);
			goto cleanup;
		}
		
		if (event.type == TB_EVENT_KEY) {
			switch (event.ch) {
			case 'q':
				active = 0;
				break;

			case 'j':
				if (MENU_MAIN.entries[cursor + 1].type !=
				    ET_NONE)
					cursor++;
				break;

			case 'k':
				if (cursor > 0)
					cursor--;
				break;
			}
		}
	}

cleanup:	
	tb_shutdown();

	return 0;
}

