#import system.

#symbol program =
[
    #var s1 := "Hello ".
    #var s2 := "World!".
    
    // Alternative 1
    console writeLine:(s1 + s2).
    
    // Alternative 2
    console writeLine:(s1 add: s2).
].