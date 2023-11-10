# What is this

The "House User Interface" is a customizable terminal user-interface for common
tasks and personal tastes, inspired in software-design by
[suckless software](http://suckless.org).  
You can statically create TUI menus in config source code and then deploy it to
your user.  
Set it as their default shell, to chain them into specific tasks :D  
A scripting interface allows you to tack logic onto the menus.  
With it you can even create entire menus at runtime.  

# Install

To install follow these steps:

- run `git clone https://github.com/SchokiCoder/hui`
- run `cd hui`
- edit [cfg/config.h][config.h]
- for experts: edit [cfg/scripts.h][scripts.h] or [config.mk][config.mk]
- run `chmod u+x *.sh; sudo ./install.sh`, optionally remove `sudo`

# HUI and Courier are bffs by default

The hui comes with it's own pager "courier".  
It is used by hui for textual feedback by default.  
Edit the variable "PAGER" in [cfg/config.h][config.h] to use something else
instead.  
To prevent `./install.sh` from compiling and installing courier, remove
"courier" from the BINARIES variable in [install.sh][install.sh].  

```Shell
BINARIES="hui courier"
```

becomes  

```Shell
BINARIES="hui"
```

# Contributing

Why thank you for even considering helping me create the most serious
application ever.  
  
Have a look at [docs/todo.md][todo.md] for what is planned short-term.  
Known bugs are also listed in there.  
Otherwise [docs/goals.md][goals.md] shows the planned long-term future.  
  
If you found something, make a fork, make your changes and open a pull request.  



[config.h]: <https://github.com/SchokiCoder/hui/blob/main/cfg/config.h>
[config.mk]: <https://github.com/SchokiCoder/hui/blob/main/config.mk>
[config.sh]: <https://github.com/SchokiCoder/hui/blob/main/config.sh>
[goals.md]: <https://github.com/SchokiCoder/hui/blob/main/docs/goals.md>
[install.sh]: <https://github.com/SchokiCoder/hui/blob/main/install.sh>
[Makefile]: <https://github.com/SchokiCoder/hui/blob/main/Makefile>
[scripts.h]: <https://github.com/SchokiCoder/hui/blob/main/cfg/scripts.h>
[todo.md]: <https://github.com/SchokiCoder/hui/blob/main/docs/todo.md>
