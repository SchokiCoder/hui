/* Copyright (C) 2022 - 2023 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#ifndef _CFG_HUI_H
#define _CFG_HUI_H

/* This here is were stuff goes, that cannot be changed during runtime.
 * Just edit the static constants.
 * The fun part is in "scripts.h". 
 */

#include "scripts.h"
#include "../color.h"
#include "../menu.h"

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
static const char *HEADER =        "- House User Interface -\n\n";
static const char *ENTRY_PREPEND = "> ";

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

#endif /* _CFG_HUI_H */
