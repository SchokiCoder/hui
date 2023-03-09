/* Copyright (C) 2022 - 2023 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#ifndef _HSTRING_H
#define _HSTRING_H

/* zero out n characters of string
 */
void strn_bleach(char *str, const long unsigned len);

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

/* removes spaces, tabs and newlines from end of the string
 */
void str_rtrim(char *str);


#endif /* _HSTRING_H */
