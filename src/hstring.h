/* Copyright (C) 2022 - 2023 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#ifndef _HSTRING_H
#define _HSTRING_H

#include <stdio.h>

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

void String_copy(struct String       *string,
		 const char          *src,
		 const long unsigned  src_len);

void
String_append(struct String       *string,
              const char          *src,
              const long unsigned  src_len);

void String_read_file(struct String *string, FILE *f);

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

/* Append char to string.
 * str:      string
 * c:        char to append
 * str_len:  len of string (excluding null-byte)
 * str_size: size of string
 * Returns 0 when ok, 1 if not enough size is given.
 */
int
strn_add_char(char                *str,
	      const char           c,
	      const long unsigned  str_len,
	      const long unsigned  str_size);

/* Counts the amount of lines needed to display a string.
 * str:       string
 * size:      size of given string
 * line_size: size that one line has
 * Returns 0, if the string is not displayable otherwise the count.
 */
long unsigned
strn_lines(const char          *str,
	   const long unsigned  size,
	   const long unsigned  line_size);

/* printf with color setting
 */
void
hprintf(const struct Color  fg,
        const struct Color  bg,
        const char         *fmt,
        ...);

#endif /* _HSTRING_H */
