/* Copyright (C) 2022 - 2023 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

/* This file contains shared behavior of hui and courier.
 */

#ifndef _COMMON_H
#define _COMMON_H

#include <termios.h>

struct Menu;
struct String;

#define SIGINT  '\003'
#define SIGTSTP '\032'

enum InputMode {
	IM_NORMAL = 0,
	IM_CMD =    1
};

static long unsigned term_x_last,
                     term_y_last;

static struct termios previous_terminal_settings;

void
draw_lower(const char           *cmdin,
           const enum InputMode  imode,
           const struct String  *feedback);

void draw_upper(long unsigned *stdout_y, const char *header, const char *title);

void
handle_cmd(const char        *cmdin,
           int               *active,
           const struct Menu *cur_menu,
	   long unsigned     *cursor,
           struct String     *feedback,
	   unsigned long     *feedback_lines);

int
handle_cmdline_opts(int          argc,
		    const char **argv,
		    const char  *app_name,
		    const char  *app_shortname,
		    const char  *app_version);

void handle_key_cmdline(const char         key,
			char              *cmdin,
			int               *active,
			enum InputMode     *imode,
			const struct Menu *cur_menu,
			long unsigned     *cursor,
			struct String     *feedback,
			unsigned long     *feedback_lines);

void
set_feedback(struct String *feedback,
             long unsigned *feedback_lines,
	     const char    *str);

void term_get_size();

void term_set_raw();

void term_restore();

#endif /* _COMMON_H */
