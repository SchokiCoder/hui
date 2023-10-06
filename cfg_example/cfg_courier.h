/* Copyright (C) 2022 - 2023 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#ifndef _CFG_COURIER_H
#define _CFG_COURIER_H

#include "../color.h"

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

#endif /* _CFG_COURIER_H */
