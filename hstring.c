/* Copyright 2022 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "color.h"
#include "hstring.h"

void strn_bleach(char *str, const long unsigned len)
{
	long unsigned i;
	
	for (i = 0; i < len; i++) {
		str[i] = '\0';
	}
}

void str_add_char(char *str, const char c)
{
	long unsigned len = strlen(str);
	
	str[len] = c;
	str[len + 1] = '\0';
}

long unsigned str_lines(char *str, long unsigned line_len)
{
	long unsigned i = 0, x = 0, ret = 1;
	
	if (str == NULL)
		return 0;
	else if (str[0] == '\0')
		return 0;

	while (str[i] != '\0') {
		if (x > line_len) {
			ret++;
			x = 0;
		}
		
		switch (str[i]) {
		case '\n':
			ret++;
			break;
			
		case '\0':
			return ret;
			break;
		
		default:
			x++;
			break;
		}
		
		i++;
	}
	
	return ret;
}

void
hprintf(const struct Color  fg,
        const struct Color  bg,
        const char         *fmt,
        ...)
{
	va_list valist;
	va_start(valist, fmt);
	
	set_fg(fg);
	set_bg(bg);
	vfprintf(stdout, fmt, valist);
	
	va_end(valist);
}

