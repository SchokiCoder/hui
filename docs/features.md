# Problems

## Keybinds
What if the user starts an application from within the hui, that has the same
keybinds (Vim, Neovim)?

- require an additional modifier key to explicitly steal a keybind
- just steal the keybinds
- gift keybinds an potentially the overall possibility of control
- just don't support mainloop-applications with keyboard-input at all
(aka undefined behavior or nothing at all)

I think option 1 is the best but option 2 is the easiest to implement.
So i will go with option 2 for now and maybe later will i change it to 1.

# Roadmap

## Configuration
The menu is configured by the user in  
/home/user/.config/menu.scm  
or (hopefully) the root-user in  
/etc/house_ui/menu.scm  
  
The home config overrides the etc config.  
GNU Guile Scheme files are used for configuration.

## User menu
Upon opening a new session or workspace the user is greeted with a menu.  
A menu has entries containing commands or submenus.

## Commands / Keybinds
The keybinds and commands are generally inspired by Vim and Neovim.  
Application defined commands are entered after pressing colon and many commands
are 1:1 compatible with Vim.  
Pressing number keys causes the next command to be executed as many times as
given unless that keybind has a specific behavior for numbers.

## Extensibility and actions
Via GNU Guile.

## I18n
Via GNU Gettext.

## Basic terminal multiplexing
Similar to GNOME's approach to workspaces.  
Two keybinds for switching back and forth through active tabs.  
As soon as a tab is not having anything open anymore and the user leaves, it
gets removed.  
A tab is active if there is multi-line output, or the user currently traverses
the menus and submenus.

## Advanced terminal multiplexing
Allows for applications with their own keyboard-input and a mainloop to run.  
However, since their keybinds could collide with my keybinds, the house_ui will
steal keybinds for now.

## Full terminal multiplexing
If this point is reached, it means house_ui will no longer steal keybinds and
instead expect a special modifier key to be pressed.  
This will explicitly steal keybinds.  
The modifier key is of course only necessary when there is currently another
application open within house_ui but it will also not refuse to work if modifier
is given regardless.

# Not planned
- login / logout and other user management
- graphical tabs

