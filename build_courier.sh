#!/bin/sh

# Copyright (C) 2022 - 2023 Andy Frank Schoknecht
# Use of this source code is governed by the BSD-3-Clause
# license, that can be found in the LICENSE file.

. "./cfg_build.sh"

"$CC" $COPTS -Os -o courier courier.c common.c color.c hstring.c sequences.c \
	-I cfg \
	$DEFINES \
	-D VERSION=\""$VERSION"\" \
	-D CONFIG_COURIER
