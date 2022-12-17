/* Copyright 2022 Andy Frank Schoknecht
 * Use of this source code is governed by the BSD-3-Clause
 * license, that can be found in the LICENSE file.
 */

#ifndef _MENU_H
#define _MENU_H

struct Entry;
struct Menu;

enum EntryType {
	ET_NONE = 0,
	ET_SUBMENU = 1,
	ET_COMMAND = 2
};

struct Entry {
	char *caption;
	
	enum EntryType type;
	const struct Menu *submenu;
	char *command;
};

struct Menu {
	char *title;
	struct Entry entries[128];
};

#endif /* _MENU_H */

