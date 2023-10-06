/* Copyright (C) 2022 - 2023 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#ifndef _CFG_COURIER_H
#define _CFG_COURIER_H

/* This here is were stuff goes, that cannot be changed during runtime.
 * Just edit the static constants.
 * The fun part is in "scripts.h". 
 */

#include "../color.h"

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

#endif /* _CFG_COURIER_H */
