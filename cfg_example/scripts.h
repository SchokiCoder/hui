/* Copyright (C) 2022 - 2024 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#ifndef _SCRIPTS_H
#define _SCRIPTS_H

#include <string.h>

#include "hstring.h"

void c_test(struct String *feedback);
void c_lucky(struct String *feedback);

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
static int lucky = 0;

#define HUI_ON_START_DATA &menu_chaos
//#define HUI_ON_QUIT_DATA  &menu_chaos

/* DEAR USER: DON'T TOUCH THE NEXT LINE... please */
#ifdef _SCRIPTS_H_IMPL

#include <stdlib.h>

void c_test(struct String *feedback)
{
	char welcome[64];
	
	counter += 1;
	
	switch (counter) {
	case 7:
		menu_chaos.entries[1].caption = "i feel lucky";
		menu_chaos.entries[1].type = ET_C;
		menu_chaos.entries[1].c = c_lucky;
		break;
	
	case 8:
		menu_chaos.entries[1].caption = NULL;
		menu_chaos.entries[1].type = ET_NONE;
		menu_chaos.entries[1].shell = NULL;
		if (lucky)
			String_append(feedback, "jk :D", 5);
			
		return;
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
	String_append(feedback, welcome, strlen(welcome));
}

void c_lucky(struct String *feedback)
{
	const char *str = "you are dumb,\nidiot";
	const long unsigned str_len = strlen(str);
	
	lucky = 1;
	String_append(feedback, str, str_len);
}

# ifdef HUI_ON_START_DATA
void hui_on_start (void *_data)
{
	struct Menu *data = _data; 
	
	if (strcmp(getenv("USER"), "andy") == 0) {
		data->entries[1].caption = "secrit";
		data->entries[1].type = ET_SHELL;
		data->entries[1].shell = "echo deeznuts";
	}
}
#endif /* HUI_ON_START_DATA */

#ifdef HUI_ON_QUIT_DATA
void hui_on_quit (void *_data)
{

}
#endif /* HUI_ON_QUIT_DATA */

#endif /* _SCRIPTS_H_IMPL */

#endif /* _SCRIPTS_H */
