/* Copyright (C) 2022 - 2023 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#ifndef _CFG_HUI_H
#define _CFG_HUI_H

#include "../color.h"
#include "../menu.h"
#include "scripts.h"

/* array sizes */
#define MENU_STACK_SIZE 64

/* key binds */
#define KEY_EXEC  'L'

/* colors */
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

/* texts and menus */
static const char *HEADER =        "Example hui\n" "\n";
static const char *ENTRY_PREPEND = "> ";

static const struct Menu MENU_FILES = {
	.title = "Files menu\n"
	         "——————————",
	.entries = {
		[0] = {
			.caption = "List home files",
			.type = ET_SHELL,
			.shell = "ls -la \"$HOME\""
		},
		
		[1] = {
			.caption = "List bin files",
			.type = ET_SHELL,
			.shell = "ls -ls /usr/bin"
		}
	}
};

static const struct Menu MENU_LONG = {
	.title = "Long menu\n"
	         "—————————",
	.entries = {
		[0] = {
			.caption = "pager SOURCE",
			.type = ET_SHELL_LONG,
			.shell = PAGER " -t \"CALLED FROM HUI CONFIG\" hui.c"
		},
		[1] = {
			.caption = "cat LICENSE",
			.type = ET_SHELL,
			.shell = "cat LICENSE"
		},
		[2] = {
			.caption = "no stdout",
			.type = ET_SHELL,
			.shell = "echo"
		},
		[3] = {
			.caption = "not working",
			.type = ET_SHELL,
			.shell = "sdasdasdasdasd"
		},
		[4] = {
			.caption = "Vi",
			.type = ET_SHELL_LONG,
			.shell = "vi ~/temp"
		},
		[5] = {
			.caption = "6",
			.type = ET_SHELL,
			.shell = "echo '6'"
		},
		[6] = {
			.caption = "7",
			.type = ET_SHELL,
			.shell = "echo '7'"
		},
		[7] = {
			.caption = "8",
			.type = ET_SHELL,
			.shell = "echo '8'"
		},
		[8] = {
			.caption = "9",
			.type = ET_SHELL,
			.shell = "echo '9'"
		},
		[9] = {
			.caption = "10",
			.type = ET_SHELL,
			.shell = "echo '10'"
		},
		[10] = {
			.caption = "11",
			.type = ET_SHELL,
			.shell = "echo '11'"
		},
		[11] = {
			.caption = "12",
			.type = ET_SHELL,
			.shell = "echo '12'"
		},
		[12] = {
			.caption = "13",
			.type = ET_SHELL,
			.shell = "echo '13'"
		},
		[13] = {
			.caption = "14",
			.type = ET_SHELL,
			.shell = "echo '14'"
		},
		[14] = {
			.caption = "15",
			.type = ET_SHELL,
			.shell = "echo '15'"
		},
		[15] = {
			.caption = "16",
			.type = ET_SHELL,
			.shell = "echo '16'"
		},
		[16] = {
			.caption = "17",
			.type = ET_SHELL,
			.shell = "echo '17'"
		},
		[17] = {
			.caption = "18",
			.type = ET_SHELL,
			.shell = "echo '18'"
		},
		[18] = {
			.caption = "19",
			.type = ET_SHELL,
			.shell = "echo '19'"
		},
		[19] = {
			.caption = "20",
			.type = ET_SHELL,
			.shell = "echo '20'"
		},
		[20] = {
			.caption = "21",
			.type = ET_SHELL,
			.shell = "echo '21'"
		}
	}
};

static const struct Menu MENU_MAIN = {
	.title = "Main menu\n"
	         "—————————",
	.entries = {
		[0] = {
			.caption = "[Chaos menu]",
			.type = ET_SUBMENU,
			.submenu = &menu_chaos
		},
		
		[1] = {
			.caption = "[Files menu]",
			.type = ET_SUBMENU,
			.submenu = &MENU_FILES
		},
		
		[2] = {
			.caption = "[Long menu]",
			.type = ET_SUBMENU,
			.submenu = &MENU_LONG
		},
		
		[3] = {
			.caption = "Show current user",
			.type = ET_SHELL,
			.shell = "echo \"$USER\""
		}
	}
};

#endif /* _CFG_HUI_H */

