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

static const struct Menu MENU_LONG = {
	.title = "Long menu\n"
	         "—————————\n",
	.entries = {
		[0] = {
			.caption = "1",
			.type = ET_COMMAND,
			.command = "echo '1'"
		},
		[1] = {
			.caption = "2",
			.type = ET_COMMAND,
			.command = "echo '2'"
		},
		[2] = {
			.caption = "3",
			.type = ET_COMMAND,
			.command = "echo '3'"
		},
		[3] = {
			.caption = "4",
			.type = ET_COMMAND,
			.command = "echo '4'"
		},
		[4] = {
			.caption = "5",
			.type = ET_COMMAND,
			.command = "echo '5'"
		},
		[5] = {
			.caption = "6",
			.type = ET_COMMAND,
			.command = "echo '6'"
		},
		[6] = {
			.caption = "7",
			.type = ET_COMMAND,
			.command = "echo '7'"
		},
		[7] = {
			.caption = "8",
			.type = ET_COMMAND,
			.command = "echo '8'"
		},
		[8] = {
			.caption = "9",
			.type = ET_COMMAND,
			.command = "echo '9'"
		},
		[9] = {
			.caption = "10",
			.type = ET_COMMAND,
			.command = "echo '10'"
		},
		[10] = {
			.caption = "11",
			.type = ET_COMMAND,
			.command = "echo '11'"
		},
		[11] = {
			.caption = "12",
			.type = ET_COMMAND,
			.command = "echo '12'"
		},
		[12] = {
			.caption = "13",
			.type = ET_COMMAND,
			.command = "echo '13'"
		},
		[13] = {
			.caption = "14",
			.type = ET_COMMAND,
			.command = "echo '14'"
		},
		[14] = {
			.caption = "15",
			.type = ET_COMMAND,
			.command = "echo '15'"
		},
		[15] = {
			.caption = "16",
			.type = ET_COMMAND,
			.command = "echo '16'"
		},
		[16] = {
			.caption = "17",
			.type = ET_COMMAND,
			.command = "echo '17'"
		},
		[17] = {
			.caption = "18",
			.type = ET_COMMAND,
			.command = "echo '18'"
		},
		[18] = {
			.caption = "19",
			.type = ET_COMMAND,
			.command = "echo '19'"
		},
		[19] = {
			.caption = "20",
			.type = ET_COMMAND,
			.command = "echo '20'"
		},
		[20] = {
			.caption = "21",
			.type = ET_COMMAND,
			.command = "echo '21'"
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
			.caption = "Long menu",
			.type = ET_SUBMENU,
			.submenu = &MENU_LONG
		},
		
		[2] = {
			.caption = "Show current user",
			.type = ET_COMMAND,
			.command = "echo \"$USER\""
		}
	}
};

#endif /* _CONFIG_H */

