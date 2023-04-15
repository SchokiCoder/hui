/* Copyright (C) 2022 - 2023 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#ifndef _TEST_SCRIPTS_H
#define _TEST_SCRIPTS_H

#include "hstring.h"

void c_test(struct String *feedback);

static struct Menu menu_chaos = {
	.title = "Chaos menu\n"
	         "———————————",
	
	.entries = {
		[0] = {
			.caption = "count welcome procedures",
			.type = ET_C,
			.c = c_test
		}
	}
};

static long unsigned counter = 0;

void c_test(struct String *feedback)
{
	char welcome[64];
	long unsigned welcome_len = 0;
	
	counter += 1;
	
	switch (counter) {
	case 7:
		menu_chaos.entries[1].caption = "i feel lucky";
		menu_chaos.entries[1].type = ET_SHELL;
		menu_chaos.entries[1].shell = "echo 'you are dumb'";
		break;
	
	case 8:
		menu_chaos.entries[1].caption = NULL;
		menu_chaos.entries[1].type = ET_NONE;
		menu_chaos.entries[1].shell = NULL;
		break;
	
	case 665:
		menu_chaos.title = "sector is secure\n"
		                   "————————————————";
		break;
	
	case 666:
		menu_chaos.title = "sector is not secure\n"
		                   "————————————————————";
		break;
	}
	
	sprintf(welcome, "Welcome to c test no: %lu", counter);
	welcome_len = strlen(welcome);
	String_append(feedback, welcome, welcome_len);
}

#endif /* _TEST_SCRIPTS_H */
