#import system.
#import system'routines.
#import extensions.

#symbol program =
[
    #var string := "Hello,How,Are,You,Today".
    
    string split &by:"," run &each:s
    [
        console write:s:".".
    ].
].