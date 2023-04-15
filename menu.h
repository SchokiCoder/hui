/* Copyright (C) 2022 - 2023 Andy Frank Schoknecht
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
	ET_NONE =    0,
	ET_SUBMENU = 1,
	ET_SHELL =   2,
	ET_C =       3
};

struct Entry {
	char              *caption;
	enum EntryType     type;
	const struct Menu *submenu;
	char              *shell;
	void              (*c) (struct String *);
};

struct Menu {
	char *title;
	struct Entry entries[MENU_MAX_ENTRIES];
};

long unsigned count_menu_entries(const struct Menu *menu);

#endif /* _MENU_H */

