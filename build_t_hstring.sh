#!/bin/sh

# Copyright (C) 2022 - 2024 Andy Frank Schoknecht
# Use of this source code is governed by the BSD-3-Clause
# license, that can be found in the LICENSE file.

. "./cfg_build.sh"

"$CC" $COPTS -g -o t_hstring \
	"$SRC_DIR/t_hstring.c" "$SRC_DIR/color.c" "$SRC_DIR/hstring.c" \
	-I "$SRC_DIR" \
	$TEST_COPTS
