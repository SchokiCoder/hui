#!/bin/sh

# Copyright (C) 2022 - 2024 Andy Frank Schoknecht
# Use of this source code is governed by the BSD-3-Clause
# license, that can be found in the LICENSE file.

LICENSE="LICENSE"
TEMP="MSG_LICENSE"
OUTPUT="src/license_header.h"

cat "$LICENSE" > "$TEMP"
printf "\0" >> "$TEMP"
xxd -i "$TEMP" > "$OUTPUT"
rm -f "$TEMP"
