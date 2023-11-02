# Compilation customizations

# compiler and compiler flags
D_CC = cc
CC = cc
COPTS = -std=c99 -pedantic

# flags, you can add to COPTS, to change some aspects:
#
# -D STRING_NOT_ON_HEAP
# Causes String struct's to be stored on stack.
# Dynamic memory allocation for strings is disabled and so they can not change
# in their size with this option.
# With this, strings are limited to their initial size.
