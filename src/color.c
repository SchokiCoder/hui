/* Copyright (C) 2022 - 2024 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#include <stdio.h>
#include "color.h"
#include "sequences.h"

void set_fg(const struct Color c)
{
	if (c.active)
		printf("\x1b[38;2;%i;%i;%im", c.r, c.g, c.b);
	else
		printf(SEQ_FG_DEFAULT);
}

void set_bg(const struct Color c)
{
	if (c.active)
		printf("\x1b[48;2;%i;%i;%im", c.r, c.g, c.b);
	else
		printf(SEQ_BG_DEFAULT);
}
