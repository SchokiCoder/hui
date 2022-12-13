# Before i can start
- what cmd line options could there be (take a look at nvim)
- what license: GPL-2.0 or Apache-2.0

# Goals
The higher a goal in the list, the higher the priority.  
A goal __can__ be ignored for fulfilling a higher goal.  
However the program can only be considered as done and reach the final release
state, once **all goals** have been fulfilled and there are **no conflicts**
between them.  
As such ignorance of any goal can be seen as a temporary solution, which needs
fixing asap and fixing this issue should become next immediate goal.  

Goal-list:  
- ANSI-C, suckless code style
- max. 4000 sloc in C (not including any config files)
- max. 200 sloc for build system (eg. make)
- strictly POSIX compatible
- no dependcies on libraries other than libc
- wether it's Linux, FreeBSD, OpenBSD or NetBSD, building is as simple as
editing the config.h and running ONE command in the root dir of the pkg
- implement all features of Roadmap and no more

As soon as all the goals have been fulfilled, the project reaches version 1.0.0
and is now only maintained for bugfixes.  
No more feature-implementations... or rewrites ;)  
except the only goal of that rewrite (wether partially or fully) is to shorten
the code and use less lines or increase portability.  
A rewrite in another language or language standard is never allowed.  

# Roadmap

## v0.0.1 Configuration
Via C source code header, suckless-style.
Provide sane, generalist standard configuration but with a compiler warning,
that is had been untouched.

## v0.1.0 User menu
Upon opening a new session or workspace the user is greeted with a menu.  
A menu has entries containing commands and submenus.
Control of fore- and background-colors is planned.
They are configurable.
The following areas can be configured (fore- and background color):
- header
- body (menu / multiline-feedback)
- feedback line
- horizontal lines
- menu entry on which the user's cursor is hovering
- internal command line

## v0.2.0 Commands / Keybinds
The keybinds and commands are generally inspired by Vim and Neovim.  
Application defined commands are entered after pressing colon and many commands
are 1:1 compatible with Vim.  
Pressing number keys causes the next command to be executed as many times as
given unless that keybind has a specific behavior for numbers.

## v0.3.0 Basic terminal multiplexing
Two keybinds for switching back and forth through active tabs.  
As soon as a tab is not having anything open anymore and the user leaves, it
gets removed.  
A tab is active if there is multi-line output, or the user currently traverses
the menus and submenus.

## 0.4.0 Advanced terminal multiplexing
Allows for applications with their own keyboard-input and a mainloop to run.  
However, since their keybinds could collide with my keybinds, the house_ui will
steal keybinds for now.

## v0.5.0 Full terminal multiplexing
If this point is reached, it means house_ui will no longer steal keybinds and
instead expect a special modifier key to be pressed.  
This will explicitly steal keybinds.  
The modifier key is of course only necessary when there is currently another
application open within house_ui but it will also not refuse to work if
modifier is given regardless.

## v0.6.0 Polished command line interface
This task is for creating/polishing the end-user cmd-line interface.  
That includes:  
- manpage
- call options (at least POSIX, maybe GNU)
- return values
- print messages (consistency, version information, license information)

# Optional goals
There are optional features which can only be implemented after the final
release state (aka version 1.0.0) has been reached.  
The features or their implementation can not contradict any non-optional goal,
if it does it can not be released.

# Optional Roadmap

## v1.1.0 Multiline feedback text: align, margin
Feedback text shall be aligned left, right or middle, depending on
configuration.  
There shall be a configurable margin for the left and right side.  

## v1.2.0 Horizontal lines
Make their existence configurable, individually.  

## v1.3.0 Path customization
Make the path delimiter configurable.  

## v1.4.0 GNU-style command line operands
Add GNU-style operands as alias to the POSIX ones.

