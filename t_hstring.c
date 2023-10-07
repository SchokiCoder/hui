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

#define TEST_CHARARR_SIZE 64
#define LINE_SIZE 32
#define BLOCK_FILL_LETTER 'a'

void test_stringfuncs()
{
	char s[TEST_CHARARR_SIZE];
	
	strn_bleach(s, TEST_CHARARR_SIZE);
	assert('\0' == s[0]);
	assert('\0' == s[TEST_CHARARR_SIZE / 2]);
	assert('\0' == s[TEST_CHARARR_SIZE - 1]);
	assert(strn_lines(s, TEST_CHARARR_SIZE, LINE_SIZE) == 0);
	
	str_add_char(s, 'E');
	assert('E' == s[0]);
	assert('\0' == s[1]);
	assert(strn_lines(s, TEST_CHARARR_SIZE, LINE_SIZE) == 1);
	
	str_add_char(s, ' ');
	str_add_char(s, '\t');
	str_add_char(s, '\n');
	assert('E' == s[0]);
	assert(' ' == s[1]);
	assert('\t' == s[2]);
	assert('\n' == s[3]);
	assert('\0' == s[4]);
	assert(strn_lines(s, TEST_CHARARR_SIZE, LINE_SIZE) == 2);

	strn_rtrim(s, TEST_CHARARR_SIZE);
	assert('E' == s[0]);
	assert('\0' == s[1]);
	assert(strn_lines(s, TEST_CHARARR_SIZE, LINE_SIZE) == 1);
	
	strncpy(s, "I am veeeeeeeeeeeery loooooooooooooong", TEST_CHARARR_SIZE);
	assert(strn_lines(s, TEST_CHARARR_SIZE, LINE_SIZE) == 2);
}

void test_stringstruct()
{
	char          block_fill[STRING_BLOCK_SIZE + 1];
	long unsigned i;
	struct String s = String_new();
	
	assert(STRING_BLOCK_SIZE == s.size);
	assert(0 == s.len);
	assert('\0' == s.str[s.len - 1]);
	
	String_copy(&s, "Foo", strlen("Foo"));
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
	
	sprintf(block_fill, "Times strings screwed me over: %i", 666);
	String_append(&s, block_fill, strlen(block_fill));
	assert(STRING_BLOCK_SIZE == s.size);
	assert(40 == s.len);
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
	String_copy(&s, block_fill, STRING_BLOCK_SIZE);
	assert((STRING_BLOCK_SIZE * 2) == s.size);
	assert(STRING_BLOCK_SIZE == s.len);
	assert('\0' == s.str[s.len]);

	String_free(&s);
	assert(0 == s.size);
	assert(0 == s.len);
	assert(NULL == s.str);
#endif
}

int main()
{
	test_stringfuncs();
	test_stringstruct();
	
	return 0;
}
