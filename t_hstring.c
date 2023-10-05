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

#define BLOCK_FILL_LETTER 'a'

int main()
{
	char          block_fill[STRING_BLOCK_SIZE + 1];
	long unsigned i;
	struct String s = String_new();
	
	assert(STRING_BLOCK_SIZE == s.size);
	assert(0 == s.len);
	assert('\0' == s.str[s.len - 1]);
	
	String_copy(&s, "Foo");
	assert(STRING_BLOCK_SIZE == s.size);
	assert(3 == s.len);
	assert('\0' == s.str[s.len]);
	
	String_append(&s, "Bar \t\n", strlen("Bar \t\n"));
	assert(STRING_BLOCK_SIZE == s.size);
	assert(9 == s.len);
	assert('\0' == s.str[s.len]);
	
	String_rtrim(&s);
	assert(STRING_BLOCK_SIZE == s.size);
	assert(6 == s.len);
	assert('\0' == s.str[s.len]);
	
	String_bleach(&s);
	assert(STRING_BLOCK_SIZE == s.size);
	assert(0 == s.len);
	assert('\0' == s.str[s.len]);

	for (i = 0; i <= STRING_BLOCK_SIZE; i++) {
		block_fill[i] = BLOCK_FILL_LETTER;
	}
	block_fill[STRING_BLOCK_SIZE] = '\0';

#ifdef STRING_NOT_ON_HEAP
	printf("note: Non-default option given, "
	       "strings are stored on stack and tests differ.\n");

	String_copy(&s, block_fill);
	assert(STRING_BLOCK_SIZE == s.size);
	assert(STRING_BLOCK_SIZE == s.len);
	assert(BLOCK_FILL_LETTER == s.str[s.len]);
#else
	String_copy(&s, block_fill);
	assert((STRING_BLOCK_SIZE * 2) == s.size);
	assert(STRING_BLOCK_SIZE == s.len);
	assert('\0' == s.str[s.len]);

	String_free(&s);
	assert(0 == s.size);
	assert(0 == s.len);
	assert(NULL == s.str);
#endif
	
	return 0;
}
