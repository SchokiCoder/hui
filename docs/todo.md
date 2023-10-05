# missing primary goals

- get stderr
  <https://stackoverflow.com/questions/30259086/how-to-use-popen>
  (fork and exec-family didn't help, system() only gives return code)
- print feedback accordingly


# goodies

- change license being compiled into binary
```Makefile
# awk
prebake_license.h:
	echo "#ifndef _PREBAKE_LICENSE_H" > $@
	echo "#define _PREBAKE_LICENSE_H" >> $@
	echo "\n/* This file is generated from the Makefile */\n" >> $@
	awk "{print \"$0\"}" LICENSE >> $@
	echo ";" >> $@
	echo "#endif /* _PREBAKE_LICENSE_H */" >> $@

# sed
prebake_license.h:
	echo "#ifndef _PREBAKE_LICENSE_H" > $@
	echo "#define _PREBAKE_LICENSE_H" >> $@
	echo "\n/* This file is generated from the Makefile */\n" >> $@
	echo "const char *MSG_LICENSE = " >> $@
	sed "s/*/\"$0\"" < LICENSE >> $@
	echo ";" >> $@
	echo "#endif /* _PREBAKE_LICENSE_H */" >> $@
```

- add config interface for custom commands ?


# 1.3.0

+ decouple reader "courier" from main hui (compiles)
	+ hui
	+ courier

+ add support for heap-free strings (spontaneous idea)

- String reinforcement
	+ add null-byte appendage to `String_append` and `String_copy`
	+ add len-check to `str_rtrim` -> `strn_rtrim`
	+ add t_hstring.c and fix till test passes
	+ test with stack-strings
	+ add tests to .gitignore and makefile:clean

- bugs
	- test hui
	- test courier
	+ hui: menu entries are not drawn (can still enter submenus)
	+ hui, draw_lower, hprintf: heap-buffer-overflow
	+ "Long menu" > "print License":
	  hui:383, hui:293, hstring:66, hstring:165; heap-buffer-overflow
	- "Chaos menu" > "i feel lucky": shows some funny stuff

- courier linker err when not using -Os, -O1 or -O2:
  "undefined reference to `c_test'"

- cfg_example/config.h: make the "pager LICENSE" entry actually call the pager
  directly

- when `handle_sh()`, hand over stdout and wait for child...
  don't we do that already?
  test with an editor called from within hui
+ add "-fsanitize=address,undefined" to debug `cc` call

- manpages
	- new for courier
	- update hui's
	- "see also" section mention courier/hui

- make sure install works
  (mind what README.md says about courier being optional)
- compat test
- set version 


--------------------------------------------------------------------------------


# goodies

- Makefile uses -Wvla, add comment why i
  use that or remove -Wvla


# 1.2.0

- fix manpage typo
- function pointer as entry cmd (as an addition to the shell string)
- runtime menu creation
  this had literally NO implementation cost :)
- add dirs for cfg targets to build sys

## Makefile changes

- add option "-Wvla" to debug
- remove "-Wimplicit-fallthrough=0" from debug
- add fall-through-comment to switch cases that fall through

## round-up

- add scripting info to manpage
- fix c feedback clear
- fix c feedback only single line
- set version


# 1.1.1

- menu.h remove unnecessary config include
- fix handle command: int size difference
- remove feedback on cmd line enter
  
- remove overall bg color
  this was a weird hack anyways,
  a real implementation would be much harder
  and it is not worth it
  
- make remove implicit fallthrough warnings
  
- set version


# 1.1.0
- configurable key binds
- configurable menu entry prepend
  
- rename license.h -> license_str.h
  cus github is weird about it
  
- set version to 1.1.0


# 1.0.0

- add arg -a (print version, license, repo)
  
- enforce yoda notation
  
- reader sigint and sigtstp just end reader
  
- split struct "runtime" into structs of data that relate to each other
  "data flow rework"
  
## string rework

- add struct
  
- realloc invalid old size on reader very long text
  and 1st char missing
  and double free
  (caused by String_append to wrong pos)

- feedback has weird characters and continues beyond actual text
  (String_append had invalid src_len)

## other

- remove draw_upper and draw_lower from draw_reader and draw_menu
  
- rename "app_menu" and "app_reader" into "amnu" + "ardr" in hui.c
  
- check for code fmt / line length
  
- return values... there are none :p
  
- print message consistency (format, stdout vs stderr)
  fix feedback on wrong cmdin
  
- std config (with compiler warning when untouched)
  
- line-break causes 1 character to disappear
  
- scrolling over a broken line pushes 1 character into the next visible line
  
- scrolling over the first visible line of a broken line pushes 1 character
  into the second visible line

- reader feedback use background color (cfg: reader_bg, reader_fg)
  
## manpage
	
- make: proper install
- fix missing color.h include
  in hstring.h for gcc and clang
- remove unused parameters and variables
- fix implicit popen and pclose declaration

## compat checks

### linux

fine.

### openbsd

- remove openbsd incompatible characters from config.h
- add different manprefix for openbsd

### freebsd

- tcc compile err:
  `/usr/include/sys/_types.h:107": error: ',' expected (got "__aligned")`
 
### windows

pffft as if
lololol

## other
	
- set version 1.0.0


# 0.3.0

## multiline feedback

- do
- also add vertical scrolling
- fix scroll limit
- fix 1st line being just empty when scrolled (newline prepended at top)
- fix str_lines not handling empty strings

## execute

- keybind
- get stdout
- get stderr
- print according to goal
- remove manually set feedback (in init_rt)

## other

- handle_key returns int for continue flagging, is it obsolete?
  yes, was at end of mainloop anyway lol.
- stdout fgets stops at newline use fread

## long feedback reader

- missing x counter reset (causes scroll-up just hiding top line and unnecessary
  line breaks)
- prints beyond string len until scroll (just replace current with mem alloc)
- new feedback doesnt replace old one
- once reader is used, it will always be used for feedback	
- reader height awareness
- reader is one line short at end
- reader after scroll 1st line is always empty
- reader can overscroll past the last line
  `rt->feedback_lines` is too high
  `str_lines()` missing x reset

- 1st character missing
- `reader_scroll` reset on exit

## other
- menu draw height awareness
	
- vertical menu scrolling
  cursor follow
	
- handle any todos or `//` comments
		
- set version 0.3.0


# 0.2.0

- fix menu open using cursor=0
- fix cursor down using main menu bounds
- code restructuring
  
## commands

- "quit", "q", "exit"
- number (move entry cursor to num)

## other

- set version 0.2.0


# 0.1.0

## compat checks

works on openbsd and freebsd via tcc.

## other
 
- feedback line
- feedback color cfg
- cmd line color cfg
- draw simplicifcation
- bg color
- handle in src todos (and `//` comments)
- cmdline prepend string cfg
- cmdline enter (':')
- cmdline leave (ctl + 'c'; enter)
- set version 0.1.0


# 0.0.1

- config struct
- draw
- get term size and print amount of empty lines accordingly
- mainloop

## keyboard ctl

- j, k (down, up)
- also cursor reaction

- l, h (right, left)
- also menu switch

## fixes

- disable stdout buf
- reset term x, y
- clear then redraw

## termbox (changes reverted)
- impl
- multichar fixes (middle score '—'), FAILED
- rollback and ditch termbox
  (a8a4e09c4bd6dac4c7df99c131bcf9ca7d57d990)

## other

- backport menu switch sys
- clear screen
- colors
- hide terminal text cursor
- draw menu cursor
- colors in config
- add version option -v
- set version 0.0.1
