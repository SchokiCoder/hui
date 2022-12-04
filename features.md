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

## Commands
Application defined commands are entered after a :  
Some examples:  
- q, quit = quit the application
- wsf, workspacefront = open new front workspace
- wsb, workspaceback = open new back workspace
- wsl, workspaceleft = go to next workspace to the left
- wsr, workspaceright = go to next workspace to the right

## Extensibility
Via GNU Guile and Scheme.

## I18n
Via GNU Gettext.

## Terminal multiplexing
Similar to GNOME's approach to workspaces.  
Two buttons for switching back and forth through active workspaces.  
The number buttons at the top of the keyboard for directly opening a specific workspace.  
As soon as a workspace is not having anything open anymore and the user leaves,  
it gets removed.  

# Not planned
- login / logout and other user management
- graphical workspaces

