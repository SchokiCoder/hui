#!/bin/sh

# Copyright (C) 2022 - 2023 Andy Frank Schoknecht
# Use of this source code is governed by the BSD-3-Clause
# license, that can be found in the LICENSE file.

. "./cfg_build.sh"
. "./build_license_header.sh"

"$D_CC" $COPTS -g -o d_hui \
	"$SRC_DIR/hui.c" "$SRC_DIR/common.c" "$SRC_DIR/color.c" \
	"$SRC_DIR/hstring.c" "$SRC_DIR/sequences.c" \
	$D_COPTS \
	-I cfg_example -I "$SRC_DIR" \
	$DEFINES \
	-D VERSION=\""$VERSION"-DEBUG\" \
	-D CONFIG_HUI
