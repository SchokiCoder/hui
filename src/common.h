/* Copyright (C) 2022 - 2024 Andy Frank Schoknecht
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

static int            is_term_raw = 0;
static struct termios previous_terminal_settings;

void
call_pager(struct String       *feedback,
	   long unsigned       *feedback_lines,
	   const long unsigned  term_y_len);

void
draw_lower(const char           *cmdin,
	   const struct String  *feedback,
	   const long unsigned   feedback_lines,
	   const enum InputMode  imode,
	   const long unsigned   term_y_len);

void draw_upper(const char          *header,
		const long unsigned  header_size,
		long unsigned       *stdout_y,
		const char          *title,
		const long unsigned  title_size,
		const long unsigned  term_x_len);

void
set_feedback(struct String       *feedback,
             long unsigned       *feedback_lines,
	     const char          *str,
	     const long unsigned  term_y_len);

void term_get_size(long unsigned *x, long unsigned *y);

void term_set_raw();

void term_restore();

#endif /* _COMMON_H */
