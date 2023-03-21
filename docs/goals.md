# Explanation: Limitations and goals 

There are limitations, primary goals and secondary goals.  

Limitations describe what is __not allowed__ in the project/program.
Examples for that could be:  

- max. 3000 sloc
- C99, Linux code style
- libc only allowed dependency
- no incompatibility with FreeBSD, OpenBSD or Linux 

All __primary goals__ are to be implemented __before the final release__, unless
the implementation of a single goal proves to be too difficult or would come
with a too high cost such as a high increase of the sloc count.
The update implementing the last primary goal, becomes the final release update,
which in semantic versioning would be 1.0.0.  
Optimally the primary goals are documented as a roadmap.  

All __secondary goals__ are to be assigned __after the final release__.  
Optimally the secondary goals are documented as a roadmap,
starting where the primary goal-roadmap ended.  

# Limitations

- C99, suckless code style
- configuration via C source code header
- max. 3000 sloc in C (not including any config files)
- max. 200 sloc for build system (eg. make)
- strictly POSIX compatible
- no dependcies on libraries other than libc
- building the package by editing config.h and config.mk and running make  
- provide secondary goal updates as patches when not that necessary  

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

stderr prints are always prepended by "ERROR ".  

## v1.0.0 Final polish

This task is for creating/polishing the end-user cmd-line interface.  
That includes:  

- manpage
- POSIX call options
- return values
- print messages (consistency, version information, license information)

Provide sane, generalist standard configuration but with a compiler warning,
that it had been untouched.

## Configurable basics

Keys:  

- command key (key that needs to be pressed and that will be displayed as first
in the command line)
- up
- down
- back
- into
- execute

Entry prepend:  
By default a menu's entry looks like this:  
> Menu entry  
  
Make the prepend "> " configurable.  

## C function interface in config (Experimental)

Each entry can have a function pointer to a function defined in the scripts.h.  
A user function has output string pointer for feedback and int return.  

## configurable aligns (Patch)

- Header
- Multiline feedback

## configurable margins (Patch)

- Header
- Multiline feedback

## Advanced sub-app execution

Allows for applications with their own keyboard-input and a mainloop to run.  
However, since their keybinds could collide with my keybinds, the house_ui will
steal keybinds for now.  

## Full sub-app execution

If this point is reached, it means house_ui will no longer steal keybinds and
instead expect a special modifier key to be pressed.  
This will explicitly steal keybinds.  
The modifier key is of course only necessary when there is currently another
application open within house_ui but it will also not refuse to work if
modifier is given unnecessarily.  

## Later if at all

- various commands, keybinds and cmdline options
- GNU-style options as aliases to the POSIX ones.
