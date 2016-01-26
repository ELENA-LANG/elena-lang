#import system.
#define system'routines.

#symbol program =
[
    #var string := "Hello,How,Are,You,Today".
    
    (string split &by:",") run &each:o
    [
        console write:(o + ".").
    ].
].