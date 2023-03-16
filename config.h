#ifndef _CONFIG_H
#define _CONFIG_H

#include "color.h"
#include "menu.h"

#warning Config not touched, remove this line and set your stuff up, champ!

#define CMD_IN_LEN 64

#define MENU_STACK_SIZE 64

static const struct Color HEADER_FG = {
	.active = -1,
	.r = 205,
	.g = 255,
	.b = 205
};

static const struct Color HEADER_BG = {
	.active = 0,
	.r = 30,
	.g = 30,
	.b = 30
};

static const struct Color TITLE_FG = {
	.active = -1,
	.r = 205,
	.g = 255,
	.b = 205
};

static const struct Color TITLE_BG = {
	.active = 0,
	.r = 30,
	.g = 30,
	.b = 30
};

static const struct Color ENTRY_FG = {
	.active = -1,
	.r = 255,
	.g = 255,
	.b = 255
};

static const struct Color ENTRY_BG = {
	.active = 0,
	.r = 30,
	.g = 30,
	.b = 30
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

static const struct Color READER_FG = {
	.active = -1,
	.r = 255,
	.g = 255,
	.b = 255
};

static const struct Color READER_BG = {
	.active = 0,
	.r = 30,
	.g = 30,
	.b = 30
};

static const struct Color FEEDBACK_FG = {
	.active = -1,
	.r = 175,
	.g = 175,
	.b = 175
};

static const struct Color FEEDBACK_BG = {
	.active = 0,
	.r = 30,
	.g = 30,
	.b = 30
};

static const struct Color CMDLINE_FG = {
	.active = -1,
	.r = 240,
	.g = 255,
	.b = 240
};

static const struct Color CMDLINE_BG = {
	.active = 0,
	.r = 30,
	.g = 30,
	.b = 30
};

static const struct Color OVERALL_BG = {
	.active = 0,
	.r = 30,
	.g = 30,
	.b = 30
};

static const char *HEADER =      "- House User Interface -\n\n";
static const char *CMD_PREPEND = ":";

static const struct Menu MENU_INFO = {
	.title = "Info\n"
	         "----",
	.entries = {
		[0] = {
			.caption = "Who am i",
			.type = ET_SHELL,
			.shell = "echo \"$USER\""
		},
		
		[1] = {
			.caption = "Hardware",
			.type = ET_SHELL,
			.shell = "lshw"
		},
	}
};

static const struct Menu MENU_MAIN = {
	.title = "Main menu\n"
	         "---------",
	.entries = {
		[0] = {
			.caption = "[Info menu]",
			.type = ET_SUBMENU,
			.submenu = &MENU_INFO
		},
		
		[1] = {
			.caption = "Power off",
			.type = ET_SHELL,
			.shell = "shutdown now"
		}
	}
};

#endif /* _CONFIG_H */
