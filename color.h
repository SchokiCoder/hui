/* Copyright 2022 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#ifndef _COLOR_H
#define _COLOR_H

#include <stdio.h>

struct Color {
	int active;
	unsigned int r;
	unsigned int g;
	unsigned int b;
};

void set_fg(const struct Color c);

void set_bg(const struct Color c);

#endif /* _COLOR_H */
