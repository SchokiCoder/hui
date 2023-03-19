# non-optional goals

So right now 1.0.0 is out and it doesn't fulfill all goals.  
*Wait, thats illegal.*  
  
Yeah, so i kinda think it is better to have limitations, primary and secondary
goals instead of goals and non-optional goals.  
This is ok, because limitations can be used for things like max sloc.  

Features however shouldn't be non-optional for 2 reasons:  

- less features -> less code
- better be imperfect than non-existant

Therefore the goals.md will be changed accordingly.  
This also doesn't feel so rigid :)  

# v0.2.0 Commands / Keybinds

Keybinds are currently implemented via a switch on a char, so it's very simple.  
Number, modifier and multichar keybinds would make it a whole lot more complex,  
so i decided to move that part of the update into the optional goals.  
Besides they are not that critical for now.  

# v0.3.0 sub-app stderr prints

I am not aware of a method to gather stdout, stderr and return code of a child
process, without stdout and stderr echoing.  
I have searched for it multiple hours.  
This still true for v1.0.0, so i move it into the optional goalset.  
