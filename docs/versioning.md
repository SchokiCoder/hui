# Versioning previously (until 1.2.X)

I used pseude-semantic versioning.  
If you consider the calling syntax of the binaries to be it's API, then it is
pseudo semantic.  
The syntax has changed in non-compatible ways without proper number increment.  
Plus hui and courier aren't really useful in the context of shell scripting.  

# Versioning now (after 1.2.X)

From now on, the versioning will be as follows:  
X.Y  
  
# X

Will be used to compress.  
Once Y would become 21, increment X and reset Y to 0.  

# Y

Will be incremented when there is a new version, regardless of it JUST fixing
bugs or bringing new features.

# Why like this?

The notion of compatibility, doesn't make too much sense for hui and courier,
unless one does only look at calling syntax but as previously mentioned this is
nonsense.  
This also counts for the "Y vs Z" in semantic versioning.  
  
Q: At which point is a fix for hui or courier breaking any
backward-compatibility?  
A: As previously written, it doesn't matter.  
  
Q: When does a fix change the behaviour of the program in a way so
that the user sees it?  
A: Always or never, it just depends on the user.  

# Transition

I will count all previous releases since 1.0.0 to the Y of the new versioning
model and go with that.  
So the first release under that new model will be "1.4".  
  
Why count since the 1.0.0 release?  
Because if applied since the beginning, the full release would have set X to 1
and Y would not have been reset before a full release.  
