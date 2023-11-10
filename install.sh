#!/bin/sh

# Copyright (C) 2022 - 2023 Andy Frank Schoknecht
# Use of this source code is governed by the BSD-3-Clause
# license, that can be found in the LICENSE file.

. "./cfg_install.sh"

BINARIES="hui courier"

for BINARY in $BINARIES; do
	./"build_$BINARY.sh"
	cp "$BINARY" "$DESTDIR$PREFIX/bin"
	chmod 755 "$DESTDIR$PREFIX/bin/$BINARY"

	./"build_man_$BINARY.sh"
	mkdir -p "$DESTDIR$MANPREFIX/man1"
	cp "$BINARY.1" "$DESTDIR$MANPREFIX/man1/$BINARY.1"
	chmod 644 "$DESTDIR$MANPREFIX/man1/$BINARY.1"
done
