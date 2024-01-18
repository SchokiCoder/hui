/* Copyright (C) 2022 - 2024 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#include <stdio.h>
#include "sequences.h"

void set_cursor(const long unsigned x, const long unsigned y)
{
	printf("\033[%lu;%luH", y, x);
}

