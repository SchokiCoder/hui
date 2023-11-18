/* Copyright (C) 2022 - 2023 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#ifndef _CONFIG_H
#define _CONFIG_H

#include "color.h"

/** common configuration **/

/* array sizes */
#define CMD_IN_SIZE 64

/* key binds */
#define KEY_UP    'k'
#define KEY_DOWN  'j'
#define KEY_LEFT  'h'
#define KEY_QUIT  'q'
#define KEY_CMD   ':'

/* colors */
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

/* texts */
static const char *CMD_PREPEND =   ":";

/** courier configuration (may overwrite common config options) **/
#ifdef CONFIG_COURIER

/* pager */
#define PAGER "./d_courier"
#define PAGER_TITLE "Courier - Feedback" /* comment out, when not using courier */

/* texts */
static const char *HEADER = "Example courier\n" "\n";

/* colors */
static const struct Color CONTENT_FG = {
	.active = -1,
	.r = 205,
	.g = 255,
	.b = 205
};

static const struct Color CONTENT_BG = {
	.active = 0,
	.r = 30,
	.g = 30,
	.b = 30
};

#endif /* CONFIG_COURIER */

/** hui configuration (may overwrite common config options) **/
#ifdef CONFIG_HUI

#include "menu.h"
#include "scripts.h"

/* pager */
#define PAGER "./d_courier"
#define PAGER_TITLE "HUI - Feedback" /* comment out, when not using courier */

/* array sizes */
#define MENU_STACK_SIZE 64

/* key binds */
#define KEY_EXEC  'L'
#define KEY_RIGHT 'l'

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
static const char *HEADER =                "Example hui\n" "\n";
static const char *ET_C_PREFIX =           "> !";
static const char *ET_C_POSTFIX =          "";
static const char *ET_SHELL_PREFIX =       "> ";
static const char *ET_SHELL_POSTFIX =      "";
static const char *ET_SHELL_LONG_PREFIX =  "> <";
static const char *ET_SHELL_LONG_POSTFIX = ">";
static const char *ET_SUBMENU_PREFIX =     "> [";
static const char *ET_SUBMENU_POSTFIX =    "]";

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
			.shell = PAGER " -t \"CALLED FROM HUI CONFIG\" README.md"
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
			.caption = "Chaos menu",
			.type = ET_SUBMENU,
			.submenu = &menu_chaos
		},
		
		[1] = {
			.caption = "Files menu",
			.type = ET_SUBMENU,
			.submenu = &MENU_FILES
		},
		
		[2] = {
			.caption = "Long menu",
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

#endif /* CONFIG_HUI */

#endif /* _CONFIG_H */
