/* Copyright 2022 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#include <stdio.h>

#include "config.h"

#define ARR_LEN(x) (sizeof(x) / sizeof(x[0]))

int main()
{
	long unsigned i;
	
	printf("%s%s\n", HEADER, MENU_MAIN.title);
	
	/* TODO this only works as long as the menu struct is cleanly allocated
	 * find out if this could hinder portability
	 */
	for (i = 0; MENU_MAIN.entries[i].type != ET_NONE; i++)
		printf("> %s\n", MENU_MAIN.entries[i].caption);
	
	/* TODO i < (term_height - entry_count) */
	for (i = 0; i < 18; i++)
		printf("\n");
	
	printf(":");

	return 0;
}

