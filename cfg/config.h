/* Copyright (C) 2022 - 2023 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#ifndef _CONFIG_H
#define _CONFIG_H

/** common configuration **/

/* This here is were stuff goes, that cannot be changed during runtime.
 * Just edit the static constants.
 * The fun part is in "scripts.h". 
 */

#include "../color.h"

#warning Config not touched, remove this line and set your stuff up, champ!

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

/* texts */
static const char *CMD_PREPEND =   ":";

/** courier configuration (may overwrite common config options) **/
#ifdef CONFIG_COURIER

/* pager */
#define PAGER "courier"
#define PAGER_TITLE "Courier - Feedback" /* comment out, when not using courier */

/* colors */
static const struct Color CONTENT_FG = {
	.active = -1,
	.r = 255,
	.g = 255,
	.b = 255
};

static const struct Color CONTENT_BG = {
	.active = 0,
	.r = 30,
	.g = 30,
	.b = 30
};

/* texts */
static const char *HEADER = "- House User Interface (Courier) -\n\n";

#endif /* CONFIG_COURIER */

/** hui configuration (may overwrite common config options) **/
#ifdef CONFIG_HUI

#include "scripts.h"
#include "../menu.h"

/* pager */
#define PAGER "courier"
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

/* texts and menus */
static const char *HEADER =                "- House User Interface -\n\n";
static const char *ET_C_PREFIX =           "> !";
static const char *ET_C_POSTFIX =          "";
static const char *ET_SHELL_PREFIX =       "> ";
static const char *ET_SHELL_POSTFIX =      "";
static const char *ET_SHELL_LONG_PREFIX =  "> <";
static const char *ET_SHELL_LONG_POSTFIX = ">";
static const char *ET_SUBMENU_PREFIX =     "> [";
static const char *ET_SUBMENU_POSTFIX =    "]";

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
			.shell = "poweroff"
		}
	}
};

#endif /* CONFIG_HUI */

#endif /* _CONFIG_H */
