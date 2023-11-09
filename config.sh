#!/bin/sh

# Copyright (C) 2022 - 2023 Andy Frank Schoknecht
# Use of this source code is governed by the BSD-3-Clause
# license, that can be found in the LICENSE file.

# paths
export PREFIX="/usr/local"
export MANPREFIX="$PREFIX/share/man"

if [ "$(uname -s)" = "OpenBSD" ]; then
	export MANPREFIX="$PREFIX/man"
fi
