/* Copyright 2022 Andy Frank Schoknecht
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

/* Returns the amount of lines measured by str len, line len and amount of
 * newline characters.
 */
long unsigned str_lines(char *str, long unsigned line_len);

/* printf with color setting
 */
void
hprintf(const struct Color  fg,
        const struct Color  bg,
        const char         *fmt,
        ...);


#endif /* _HSTRING_H */
