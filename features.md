# Roadmap

## Configuration
The menu is configured by the user  
via /home/user/.config/menu.scm  
or the root-user  
via /etc/house_ui/menu.scm  
  
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
Via GNU Guile and Scheme.

## I18n
Via GNU Gettext.

## Terminal multiplexing
Similar to GNOME's approach to workspaces.  
Two buttons for switching back and forth through active tabs.  
The number buttons at the top of the keyboard for directly opening a specific
tab.  
As soon as a tab is not having anything open anymore and the user leaves, it
gets removed.  

# Not planned
- login / logout and other user management
- graphical workspaces
