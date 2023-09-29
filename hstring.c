/* Copyright (C) 2022 - 2023 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "color.h"
#include "hstring.h"

struct String String_new()
{
	struct String ret = {
		.size = STRING_BLOCK_SIZE,
		.len = 0,
		.str = malloc(STRING_BLOCK_SIZE)
	};
	
	ret.str[0] = '\0';
	
	return ret;
}

void String_copy(struct String *string, const char *src)
{
	const long unsigned src_len = strlen(src);
	
	while (src_len > string->size) {
		String_grow(string);
	}
	
	strncpy(string->str, src, src_len);
	string->len = src_len;
}

void
String_append(struct String       *string,
              const char          *src,
              const long unsigned  src_len)
{
	const long unsigned new_len = src_len + string->len;
	
	while (new_len > string->size) {
		String_grow(string);
	}
	
	strncpy(&string->str[string->len], src, src_len);
	string->len = new_len;
}

void String_rtrim(struct String *string)
{
	string->len = str_rtrim(string->str); 
}

void String_grow(struct String *string)
{
	string->size += STRING_BLOCK_SIZE;
	string->str = realloc(string->str, string->size);
}

void String_bleach(struct String *string)
{
	strn_bleach(string->str, string->len);
	string->len = 0;
}

void String_free(struct String *string)
{
	free(string->str);
}

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

long unsigned str_lines(const char *str, long unsigned line_len)
{
	long unsigned i = 0, x = 0, ret = 1;
	
	if (NULL == str || '\0' == str[0])
		return 0;

	while (str[i] != '\0') {
		if (x > line_len) {
			ret += 1;
			x = 0;
		}
		
		switch (str[i]) {
		case '\n':
			ret += 1;
			x = 0;
			break;
			
		case '\0':
			return ret;
			break;
		
		default:
			x += 1;
			break;
		}
		
		i += 1;
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

long unsigned str_rtrim(char *str)
{
	long unsigned pos = 0;
	
	while (str[pos] != '\0')
		pos += 1;

	do
		pos -= 1;
	while (' ' == str[pos] || '\t' == str[pos] || '\n' == str[pos]);
	
	pos += 1;
	
	str[pos] = '\0';
	
	return pos;
}

