/* Copyright (C) 2022 - 2023 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

/* This file tests the struct String from hstring.
 */

#include <assert.h>
#include <hstring.h>
#include <stdio.h>
#include <string.h>

#define LINE_SIZE 24

void strn_lines_on_filled()
{
	char s[64];

	strncpy(s, "I am veeeeeeeeeeeery loooooooooooooong", sizeof(s));
	assert(strn_lines(s, sizeof(s), LINE_SIZE) == 2);
}

void strn_bleach_on_uninitialized()
{
	char s[16];

	strn_bleach(s, sizeof(s));
	assert('\0' == s[0]);
	assert('\0' == s[sizeof(s) / 2]);
	assert('\0' == s[sizeof(s) - 1]);
	assert(strn_lines(s, sizeof(s), LINE_SIZE) == 0);
}

void strn_add_char_on_bleached()
{
	char s[16];

	strn_bleach(s, sizeof(s));
	assert(strn_add_char(s, 'E', strlen(s), sizeof(s)) == 0);
	assert('E' == s[0]);
	assert('\0' == s[1]);
	assert(strn_lines(s, sizeof(s), LINE_SIZE) == 1);
}

void strn_add_char_multiple_on_initialized()
{
	char s[16];

	strn_bleach(s, sizeof(s));
	assert(strn_add_char(s, 'E', strlen(s), sizeof(s)) == 0);
	assert(strn_add_char(s, ' ', strlen(s), sizeof(s)) == 0);
	assert(strn_add_char(s, '\t', strlen(s), sizeof(s)) == 0);
	assert(strn_add_char(s, '\n', strlen(s), sizeof(s)) == 0);
	assert('E' == s[0]);
	assert(' ' == s[1]);
	assert('\t' == s[2]);
	assert('\n' == s[3]);
	assert('\0' == s[4]);
	assert(strn_lines(s, sizeof(s), LINE_SIZE) == 2);
}

void strn_rtrim_on_filled()
{
	char s[16];

	strn_bleach(s, sizeof(s));
	assert(strn_add_char(s, 'E', strlen(s), sizeof(s)) == 0);
	assert(strn_add_char(s, ' ', strlen(s), sizeof(s)) == 0);
	assert(strn_add_char(s, '\t', strlen(s), sizeof(s)) == 0);
	assert(strn_add_char(s, '\n', strlen(s), sizeof(s)) == 0);
	strn_rtrim(s, sizeof(s));
	assert('E' == s[0]);
	assert('\0' == s[1]);
	assert(strn_lines(s, sizeof(s), LINE_SIZE) == 1);
}

void strn_rtrim_on_empty()
{
	char s[16];

	strn_bleach(s, sizeof(s));
	strn_rtrim(s, sizeof(s));
	assert('\0' == s[0]);
	assert(strn_lines(s, sizeof(s), LINE_SIZE) == 0);
}

void strn_rtrim_on_visually_empty()
{
	char s[16];

	strn_bleach(s, sizeof(s));
	assert(strn_add_char(s, ' ', strlen(s), sizeof(s)) == 0);
	assert(strn_add_char(s, '\t', strlen(s), sizeof(s)) == 0);
	assert(strn_add_char(s, '\n', strlen(s), sizeof(s)) == 0);
	strn_rtrim(s, sizeof(s));
	assert('\0' == s[0]);
	assert(strn_lines(s, sizeof(s), LINE_SIZE) == 0);
}

void strn_add_char_on_maxed()
{
	long unsigned i;
	char s[16];
	
	for (i = 0; i < (sizeof(s) - 1); i++) {
		s[i] = 'A';
	}
	s[sizeof(s) - 1] = '\0';
	assert(strn_add_char(s, 'N', strlen(s), sizeof(s)) == 1);
}

void String_new_nothing_else()
{
	struct String s;

	s = String_new();
	assert(STRING_BLOCK_SIZE == s.size);
	assert(0 == s.len);
	assert('\0' == s.str[s.len - 1]);

#ifndef STRING_NOT_ON_HEAP
	String_free(&s);
#endif
}

void String_copy_on_initialized()
{
	struct String s;

	s = String_new();
	String_copy(&s, "Foo", strlen("Foo"));
	assert(STRING_BLOCK_SIZE == s.size);
	assert(3 == s.len);
	assert('\0' == s.str[s.len]);

#ifndef STRING_NOT_ON_HEAP
	String_free(&s);
#endif
}

void String_append_on_filled()
{
	struct String s;

	s = String_new();
	String_copy(&s, "Foo", strlen("Foo"));
	String_append(&s, "Bar \t\n", strlen("Bar \t\n"));
	assert(STRING_BLOCK_SIZE == s.size);
	assert(9 == s.len);
	assert('\0' == s.str[s.len]);

#ifndef STRING_NOT_ON_HEAP
	String_free(&s);
#endif
}

void String_rtrim_on_filled()
{
	struct String s;

	s = String_new();
	String_copy(&s, "FooBar \t\n", strlen("FooBar \t\n"));
	String_rtrim(&s);
	assert(STRING_BLOCK_SIZE == s.size);
	assert(6 == s.len);
	assert('\0' == s.str[s.len]);

#ifndef STRING_NOT_ON_HEAP
	String_free(&s);
#endif
}

void String_rtrim_on_initialized()
{
	struct String s;

	s = String_new();
	String_rtrim(&s);
	assert(STRING_BLOCK_SIZE == s.size);
	assert(0 == s.len);
	assert('\0' == s.str[s.len]);

#ifndef STRING_NOT_ON_HEAP
	String_free(&s);
#endif
}

void String_rtrim_on_visually_empty()
{
	struct String s;

	s = String_new();
	String_copy(&s, " \t\n", strlen(" \t\n"));
	String_rtrim(&s);
	assert(STRING_BLOCK_SIZE == s.size);
	assert(0 == s.len);
	assert('\0' == s.str[s.len]);

#ifndef STRING_NOT_ON_HEAP
	String_free(&s);
#endif
}

void String_append_array_on_initialized()
{
	char          pain[64];
	struct String s;

	sprintf(pain, "Times strings screwed me over: %i", 666);

	s = String_new();
	String_append(&s, pain, strlen(pain));
	assert(STRING_BLOCK_SIZE == s.size);
	assert(34 == s.len);
	assert('\0' == s.str[s.len]);

#ifndef STRING_NOT_ON_HEAP
	String_free(&s);
#endif
}

void String_bleach_on_filled()
{
	struct String s;

	s = String_new();
	String_copy(&s, "FooBar", strlen("FooBar"));
	String_bleach(&s);
	assert(STRING_BLOCK_SIZE == s.size);
	assert(0 == s.len);
	assert('\0' == s.str[s.len]);

#ifndef STRING_NOT_ON_HEAP
	String_free(&s);
#endif
}

void String_copy_block_fill_on_initialized()
{
	char          block_fill[STRING_BLOCK_SIZE + 1];
	const char    block_fill_char = 'a';
	long unsigned i;
	struct String s;

	for (i = 0; i <= STRING_BLOCK_SIZE; i++)
		block_fill[i] = block_fill_char;
	block_fill[STRING_BLOCK_SIZE] = '\0';

	s = String_new();
	String_copy(&s, block_fill, strlen(block_fill));
#ifdef STRING_NOT_ON_HEAP
	assert(STRING_BLOCK_SIZE == s.size);
	assert(STRING_BLOCK_SIZE == s.len);
	assert(block_fill_char == s.str[s.len - 1]);
#else
	assert((STRING_BLOCK_SIZE * 2) == s.size);
	assert(STRING_BLOCK_SIZE == s.len);
	assert(block_fill_char == s.str[s.len - 1]);
	assert('\0' == s.str[s.len]);

	String_free(&s);
#endif
}

void String_free_on_filled()
{
	const char    *a = "*chuckles* I'm in danger\n";
	struct String  s;

	s = String_new();
	String_copy(&s, a, strlen(a));
	String_free(&s);

#ifdef STRING_NOT_ON_HEAP
	assert(STRING_BLOCK_SIZE == s.size);
	assert(NULL != s.str);
#else
	assert(0 == s.size);
	assert(NULL == s.str);
#endif
	assert(0 == s.len);
}

void String_append_overfill_on_initialized()
{
	long unsigned i;
	char          overfill[STRING_BLOCK_SIZE * 2];
	const char    overfill_char = 'a';
	struct String s;

	for (i = 0; i < sizeof(overfill) - 1; i++)
		overfill[i] = overfill_char;
	overfill[sizeof(overfill) - 1] = '\0';

	s = String_new();	
	String_append(&s, overfill, sizeof(overfill));

#ifdef STRING_NOT_ON_HEAP
	assert(STRING_BLOCK_SIZE == s.size);
	assert(sizeof(overfill) == s.len);
	assert(overfill_char == s.str[s.size - 1]);
#else
	assert(STRING_BLOCK_SIZE * 3 == s.size);
	assert(sizeof(overfill) == s.len);
	assert(overfill_char == s.str[sizeof(overfill) - 2]);
	assert('\0' == s.str[sizeof(overfill) - 1]);

	String_free(&s);
#endif
}

int main()
{
	strn_lines_on_filled();
	strn_bleach_on_uninitialized();
	strn_add_char_on_bleached();
	strn_add_char_multiple_on_initialized();
	strn_rtrim_on_filled();
	strn_rtrim_on_empty();
	strn_rtrim_on_visually_empty();
	strn_add_char_on_maxed();
	
	String_new_nothing_else();
	String_copy_on_initialized();
	String_append_on_filled();
	String_rtrim_on_filled();
	String_rtrim_on_initialized();
	String_rtrim_on_visually_empty();
	String_append_array_on_initialized();
	String_bleach_on_filled();

#ifdef STRING_NOT_ON_HEAP
	printf("note: Non-default option given, "
	       "strings are stored on stack and tests differ.\n");
#endif

	String_free_on_filled();
	String_copy_block_fill_on_initialized();
	String_append_overfill_on_initialized();
	
	return 0;
}
