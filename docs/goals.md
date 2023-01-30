# Goals

The higher a goal in the list, the higher the priority.  
A goal __can__ be ignored for fulfilling a higher goal.  
However the program can only be considered as done and reach the final release
state, once **all non-optional goals** have been fulfilled and there are
**no conflicts** between them.  
As such ignorance of any goal can be seen as a temporary solution, which needs
fixing asap and fixing this issue should become the next short-term goal.  

Long-term non-optional goal-list:  

- C99, suckless code style
- configuration via C source code header
- max. 3000 sloc in C (not including any config files)
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

## v0.1.0 User menu

Upon start, the user is greeted with a menu.  
A menu has entries containing commands and submenus.  
Every menu has it's own custom title, which is displayed under the header.  
Use of fore- and background-colors are planned.  
They are configurable.  
The following areas can be configured (fore- and background color):  

- header
- title
- body (menu / multiline-feedback)
- feedback line
- menu entry on which the user's cursor is hovering
- internal command line

If the implementation of color control is too cumbersome, complicated,
line-heavy or unportable, then it can be pushed into the front of the optional
roadmap of features.  

## v0.2.0 Commands / Keybinds

The keybinds and commands are generally inspired by Vim and Neovim.  
Application defined commands are entered after pressing colon and many commands
are 1:1 compatible with Vim.  
Pressing number keys causes the next command to be executed as many times as
given unless that keybind has a specific behavior for numbers.

## v0.3.0 Basic sub-app execution

Execute another app, which prints to stdout and redirect that output to it's
own feedback.  

If sub-app prints to stderr:

- if stderr multineline: display only stderr in multiline feedback
- if stderr one line: display it in commandline and stdout in multiline feedback

stderr prints is always prepended by "ERROR ".  

## v1.0.0 Final polish

This task is for creating/polishing the end-user cmd-line interface.  
That includes:  

- manpage
- POSIX call options
- return values
- print messages (consistency, version information, license information)

Provide sane, generalist standard configuration but with a compiler warning,
that it had been untouched.

# Optional goals

There are optional features which can only be implemented after the final
release state (aka version 1.0.0) has been reached.  
The features or their implementation can not contradict any non-optional goal,
if it does it can not be released.

# Optional Roadmap

## v1.1.0 Header: configurable align

## v1.2.0 GNU-style command line operands

Add GNU-style options as aliases to the POSIX ones.

## v1.3.0 Configurable keys

- command key (key that needs to be pressed and that will be displayed as first
in the command line)
- up
- down
- back
- into
- execute
- steal / force

## v1.4.0 Multiline feedback: configurable (align, left and right margin)

## v1.5.0 Title: configurable (align, left and right margin)

## v1.6.0 Configurable menu entry prepend

By default a menu's entry looks like this:  
> Menu entry  
  
Make the prepend "> " configurable.  

## 1.7.0 Advanced sub-app execution

Allows for applications with their own keyboard-input and a mainloop to run.  
However, since their keybinds could collide with my keybinds, the house_ui will
steal keybinds for now.  

## v1.8.0 Full sub-app execution

If this point is reached, it means house_ui will no longer steal keybinds and
instead expect a special modifier key to be pressed.  
This will explicitly steal keybinds.  
The modifier key is of course only necessary when there is currently another
application open within house_ui but it will also not refuse to work if
modifier is given unnecessarily.  

## Later

After all other things had been done, these are next:  

- various commands, keybinds and cmdline options

