#import system.
#define extensions.
#define extensions'routines.

#symbol program =
[
    #var s := "Mary had a @@x lamb." replace &literal:"@@x" &literal:"little".
    console writeLine:s.
].