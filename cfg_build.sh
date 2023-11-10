#!/bin/sh

# Copyright (C) 2022 - 2023 Andy Frank Schoknecht
# Use of this source code is governed by the BSD-3-Clause
# license, that can be found in the LICENSE file.

# Compilation customizations

# compiler and compiler flags
export D_CC="cc"
export CC="cc"
export COPTS="-std=c99 -pedantic"

# lines you can uncomment, to add flags to COPTS, which change some aspects:
#
# Causes String struct's to be stored on stack.
# Dynamic memory allocation for strings is disabled and so they can not change
# in their size with this option.
# With this, strings are limited to their initial size.
#
#export COPTS="$COPTS -D STRING_NOT_ON_HEAP"



# Dear user, DON'T TOUCH anything beyond this point, devs only zone

export VERSION="1.4rc"
export DEFINES="-D _DEFAULT_SOURCE -D _BSD_SOURCE -D _POSIX_C_SOURCE=200809L"

# no vla's (bigger binary and unnecessary)
export D_COPTS="-Wall -Wextra -Wvla -Wno-unused-variable -fsanitize=address,undefined"
export TEST_COPTS="-Wall -Wextra -Wvla -I ."
