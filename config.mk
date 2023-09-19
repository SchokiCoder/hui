# Compilation customizations

# compiler and compiler flags
D_CC = zig cc
CC = cc
COPTS = -std=c99 -pedantic

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

# OpenBSD (uncomment)
#MANPREFIX = ${PREFIX}/man
