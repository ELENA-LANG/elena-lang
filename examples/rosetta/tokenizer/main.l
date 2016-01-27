#import system.
#import system'routines.

#symbol program =
[
    #var string := "Hello,How,Are,You,Today".
    
    #var output := string split &by:"," reverse.

    string split &by:"," run &each:s
    [
        console write:(s + ".").
    ].
].