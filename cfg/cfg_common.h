/* Copyright (C) 2022 - 2023 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#ifndef _CFG_COMMON_H
#define _CFG_COMMON_H

/* This here is were stuff goes, that cannot be changed during runtime.
 * Just edit the static constants.
 * The fun part is in "scripts.h". 
 */

#include "../color.h"

#warning Config not touched, remove this line and set your stuff up, champ!

/* array sizes */
#define CMD_IN_LEN 64

/* key binds */
#define KEY_UP    'k'
#define KEY_DOWN  'j'
#define KEY_LEFT  'h'
#define KEY_RIGHT 'l'
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

#endif /* _CFG_COMMON_H */
