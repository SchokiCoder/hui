#!/bin/sh

# Copyright (C) 2022 - 2024 Andy Frank Schoknecht
# Use of this source code is governed by the BSD-3-Clause
# license, that can be found in the LICENSE file.

. "./cfg_build.sh"
. "./build_license_header.sh"

"$CC" $COPTS -Os -o courier \
	"$SRC_DIR/courier.c" "$SRC_DIR/common.c" "$SRC_DIR/color.c" \
	"$SRC_DIR/hstring.c" "$SRC_DIR/sequences.c" \
	-I cfg -I "$SRC_DIR" \
	$DEFINES \
	-D VERSION=\""$VERSION"\" \
	-D CONFIG_COURIER
