#!/bin/sh

# Copyright (C) 2022 - 2023 Andy Frank Schoknecht
# Use of this source code is governed by the BSD-3-Clause
# license, that can be found in the LICENSE file.

. "./config.sh"

BINARIES="hui courier"

for BINARY in $BINARIES; do
	make "$BINARY"
	cp "$BINARY" "$DESTDIR$PREFIX/bin"
	chmod 755 "$DESTDIR$PREFIX/bin/$BINARY"
done

make hui.1
mkdir -p "$DESTDIR$MANPREFIX/man1"
cp hui.1 "$DESTDIR$MANPREFIX/man1/hui.1"
chmod 644 "$DESTDIR$MANPREFIX/man1/hui.1"
