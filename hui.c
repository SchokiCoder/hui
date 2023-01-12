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
		tb_set_cell(tc->x, tc->y, str[i], fg, bg);
		tc->x++;

		if (tc->x > tb_width() || str[i] == '\n') {
			tc->x = 0;
			tc->y++;
		}
	}
}

void draw_clear(uint16_t fg, uint16_t bg)
{
	long unsigned x, y;
	
	for (x = 0; x < tb_width(); x++) {
		for (y = 0; y < tb_height(); y++) {
			tb_set_cell(x, y, ' ', fg, bg);
		}
	}
}

void
draw_menu(struct TermCursor *tc, const char *header, const struct Menu *menu,
	  const long unsigned cursor)
{
	long unsigned i;
	uint16_t fg, bg;

	//hprint(tc, header, TB_WHITE, TB_DEFAULT);
	tb_printf(tc->x, tc->y++, 0, 0, header);
	//hprint(tc, menu->title, TB_WHITE, TB_DEFAULT);
	tb_printf(tc->x, tc->y++, 0, 0, menu->title);
	//hprint(tc, "\n", TB_WHITE, TB_DEFAULT);

	for (i = 0; menu->entries[i].type != ET_NONE; i++) {
		if (i == cursor) {
			fg = TB_DEFAULT;
			bg = TB_WHITE;
		}
		else {
			fg = TB_WHITE;
			bg = TB_DEFAULT;
		}
		
		//hprint(tc, "> ", fg, bg);
		//hprint(tc, menu->entries[i].caption, fg, bg);
		//hprint(tc, "\n", fg, bg);
		tb_printf(tc->x, tc->y++, fg, bg, "> %s", menu->entries[i].caption);
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
	
	long unsigned menu_stack_len = 1;
	const struct Menu *menu_stack[MENU_STACK_SIZE] = {
		[0] = &MENU_MAIN
	};
	const struct Menu *cur_menu = &MENU_MAIN;

	if (tb_init() < 0) {
		printf("Termbox init failed");
		goto cleanup;
	}
	
	tb_set_output_mode(TB_OUTPUT_NORMAL);

	while (active) {
		tc.x = 0;
		tc.y = 0;
		
		//draw_clear(TB_WHITE, TB_DEFAULT);
		tb_clear();
		
		draw_menu(&tc, HEADER, cur_menu, cursor);
		
		tb_present();

		/* key handling */
		if (tb_poll_event(&event) == -1) {
			hprint(&tc, "Poll event failed", TB_RED, TB_DEFAULT);
			goto cleanup;
		}
		
		/* if no keypress, skip */
		if (event.type != TB_EVENT_KEY)
			continue;
		
		switch (event.ch) {
		case 'q':
			active = 0;
			break;

		case 'j':
			if (cur_menu->entries[cursor + 1].type != ET_NONE)
				cursor++;
			break;

		case 'k':
			if (cursor > 0)
				cursor--;
			break;

		case 'l':
			if (cur_menu->entries[cursor].type == ET_SUBMENU) {
				cursor = 0;
				cur_menu = cur_menu->entries[cursor].submenu;
				menu_stack[menu_stack_len] = cur_menu;
				menu_stack_len++;
			}
			break;
		
		case 'h':
			if (menu_stack_len > 1) {
				cursor = 0;
				menu_stack_len--;
				cur_menu = menu_stack[menu_stack_len - 1];
			}
			break;
		}
	}

cleanup:	
	tb_shutdown();

	return 0;
}

