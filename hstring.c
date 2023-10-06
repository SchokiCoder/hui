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
#ifndef STRING_NOT_ON_HEAP
		.str = malloc(STRING_BLOCK_SIZE)
#endif
	};
	
	ret.str[0] = '\0';
	
	return ret;
}

void String_copy(struct String       *string,
		 const char          *src,
		 const long unsigned  src_len)
{
	string->len = 0;
	String_append(string, src, src_len);
}

void
String_append(struct String       *string,
              const char          *src,
              const long unsigned  src_len)
{
	const long unsigned new_len = src_len + string->len;
	
	while ((new_len + 1) > string->size) {
		String_grow(string);

#ifdef STRING_NOT_ON_HEAP
		break;
#endif
	}
	
	strncpy(&string->str[string->len], src, src_len);
	string->len = new_len;
	string->str[string->len] = '\0';
}

void String_rtrim(struct String *string)
{
	string->len = strn_rtrim(string->str, string->len);
}

void String_grow(struct String *string)
{
#ifdef STRING_NOT_ON_HEAP
	fprintf(stderr, "hstring: Strings not on heap, can't grow.\n");
#else
	string->size += STRING_BLOCK_SIZE;
	string->str = realloc(string->str, string->size);
#endif
}

void String_bleach(struct String *string)
{
	strn_bleach(string->str, string->len);
	string->len = 0;
}

void String_free(struct String *string)
{
#ifdef STRING_NOT_ON_HEAP
	fprintf(stderr, "hstring: Strings not on heap, can't free.\n");
#else
	free(string->str);
	string->str = NULL;
	string->size = 0;
#endif

	string->len = 0;
}

void strn_bleach(char *str, const long unsigned len)
{
	long unsigned i;
	
	for (i = 0; i < len; i++) {
		str[i] = '\0';
	}
}

long unsigned strn_rtrim(char *str, const long unsigned size)
{
	long unsigned pos;
	
	for (pos = 0; str[pos] != '\0'; pos++) {
		if (pos >= size) {
			return pos;
		}
	}
	
	if (0 == pos)
		return pos;

	do
		pos -= 1;
	while (' ' == str[pos] || '\t' == str[pos] || '\n' == str[pos]);
	
	pos += 1;
	
	str[pos] = '\0';
	
	return pos;
}

void str_add_char(char *str, const char c)
{
	long unsigned len = strlen(str);
	
	str[len] = c;
	str[len + 1] = '\0';
}

long unsigned
strn_lines(const char          *str,
	   const long unsigned  size,
	   const long unsigned  line_size)
{
	long unsigned i,
		      x = 0,
		      ret = 1;
	
	if (NULL == str || '\0' == str[0])
		return 0;

	for (i = 0; i < size; i++) {
		if (str[i] == '\0')
			break;
		
		if (x > line_size) {
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
