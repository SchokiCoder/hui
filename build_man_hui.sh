#!/bin/sh

# Copyright (C) 2022 - 2024 Andy Frank Schoknecht
# Use of this source code is governed by the BSD-3-Clause
# license, that can be found in the LICENSE file.

. "./cfg_build.sh"

sed "s/VERSION/$VERSION/g" < docs/hui.1 > hui.1
