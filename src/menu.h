/* Copyright (C) 2022 - 2024 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#ifndef _MENU_H
#define _MENU_H

#define MENU_MAX_ENTRIES 128

struct Entry;
struct Menu;
struct String;

enum EntryType {
	ET_NONE =       0,
	ET_SUBMENU =    1, /* leads to other menu */
	ET_SHELL =      2, /* immediate shell command */
	ET_SHELL_LONG = 3, /* long time shell command with it's own mainloop */
	ET_C =          4  /* c function pointer */
};

struct Entry {
	char              *caption;
	enum EntryType     type;
	const struct Menu *submenu;
	char              *shell;
	void              (*c) (struct String *feedback);
};

struct Menu {
	char *title;
	struct Entry entries[MENU_MAX_ENTRIES];
};

long unsigned count_menu_entries(const struct Menu *menu);

#endif /* _MENU_H */

