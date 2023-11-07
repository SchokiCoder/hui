#!/bin/sh

# Copyright (C) 2022 - 2023 Andy Frank Schoknecht
# Use of this source code is governed by the BSD-3-Clause
# license, that can be found in the LICENSE file.

. "./config.sh"

rm "$DESTDIR$PREFIX/bin/hui" \
	"$DESTDIR$PREFIX/bin/courier" \
	"$DESTDIR$MANPREFIX/man1/hui.1" \
	"$DESTDIR$MANPREFIX/man1/courier.1"
