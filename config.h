/* Copyright 2022 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#ifndef _CONFIG_H
#define _CONFIG_H

#include "color.h"
#include "menu.h"

#define CMD_IN_MAX_LEN 64

#define MENU_STACK_SIZE 64

static const struct Color HEADER_FG = {
	.active = -1,
	.r = 255,
	.g = 255,
	.b = 255
};

static const struct Color HEADER_BG = {
	.active = 0,
	.r = 0,
	.g = 0,
	.b = 0
};

static const struct Color TITLE_FG = {
	.active = -1,
	.r = 255,
	.g = 255,
	.b = 255
};

static const struct Color TITLE_BG = {
	.active = 0,
	.r = 0,
	.g = 0,
	.b = 0
};

static const struct Color ENTRY_FG = {
	.active = -1,
	.r = 255,
	.g = 255,
	.b = 255
};

static const struct Color ENTRY_BG = {
	.active = 0,
	.r = 0,
	.g = 0,
	.b = 0
};

static const struct Color ENTRY_HOVER_FG = {
	.active = -1,
	.r = 0,
	.g = 0,
	.b = 0
};

static const struct Color ENTRY_HOVER_BG = {
	.active = -1,
	.r = 255,
	.g = 255,
	.b = 255
};

static const struct Color FEEDBACK_FG = {
	.active = -1,
	.r = 175,
	.g = 175,
	.b = 175
};

static const struct Color FEEDBACK_BG = {
	.active = 0,
	.r = 0,
	.g = 0,
	.b = 0
};

static const struct Color CMDLINE_FG = {
	.active = -1,
	.r = 240,
	.g = 255,
	.b = 240
};

static const struct Color CMDLINE_BG = {
	.active = 0,
	.r = 0,
	.g = 0,
	.b = 0
};

static const struct Color OVERALL_BG = {
	.active = 0,
	.r = 0,
	.g = 20,
	.b = 0
};

static const char *HEADER =      "Maintenance tools\n" "\n";
static const char *CMD_PREPEND = ":";

static const struct Menu MENU_FILES = {
	.title = "Files menu\n"
	         "——————————",
	.entries = {
		[0] = {
			.caption = "List home files",
			.type = ET_COMMAND,
			.command = "ls -la \"$HOME\""
		},
		[1] = {
			.caption = "List bin files",
			.type = ET_COMMAND,
			.command = "ls -ls /usr/bin"
		}
	}
};

static const struct Menu MENU_MAIN = {
	.title = "Main menu\n"
	         "—————————",
	.entries = {
		[0] = {
			.caption = "Files menu",
			.type = ET_SUBMENU,
			.submenu = &MENU_FILES
		},
		
		[1] = {
			.caption = "Show current user",
			.type = ET_COMMAND,
			.command = "echo \"$USER\""
		}
	}
};

#endif /* _CONFIG_H */

