#!/bin/sh

# Copyright (C) 2022 - 2023 Andy Frank Schoknecht
# Use of this source code is governed by the BSD-3-Clause
# license, that can be found in the LICENSE file.

. "./cfg_build.sh"

"$D_CC" $COPTS -g -o d_courier courier.c common.c color.c hstring.c sequences.c \
	$D_COPTS \
	-I cfg_example \
	$DEFINES \
	-D VERSION=\""$VERSION"-DEBUG\" \
	-D CONFIG_COURIER
