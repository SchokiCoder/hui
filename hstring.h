/* Copyright (C) 2022 - 2023 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#ifndef _HSTRING_H
#define _HSTRING_H

#include "color.h"

#define STRING_BLOCK_SIZE 2048

struct String {
#ifdef STRING_NOT_ON_HEAP
	const long unsigned size;
#else
	long unsigned size;
#endif

	long unsigned len;

#ifdef STRING_NOT_ON_HEAP
	char str[STRING_BLOCK_SIZE];
#else
	char *str;
#endif
};

struct String String_new();

void String_copy(struct String *string, const char *src);

void
String_append(struct String       *string,
              const char          *src,
              const long unsigned  src_len);

void String_rtrim(struct String *string);

void String_grow(struct String *string);

void String_bleach(struct String *string);

void String_free(struct String *string);

/* zero out n characters of string
 */
void strn_bleach(char *str, const long unsigned len);

/* Removes spaces, tabs and newlines from end of the string.
 * str:  string
 * size: size of given string
 * Returns the new length of the string or the size if no null-byte was found.
 */
long unsigned strn_rtrim(char *str, const long unsigned size);

/* append char to string
 */
void str_add_char(char *str, const char c);

/* Returns:
 * the amount of lines needed to display given string
 * 0 - if the string is not displayable
 */
long unsigned str_lines(const char *str, long unsigned line_len);

/* printf with color setting
 */
void
hprintf(const struct Color  fg,
        const struct Color  bg,
        const char         *fmt,
        ...);

#endif /* _HSTRING_H */
